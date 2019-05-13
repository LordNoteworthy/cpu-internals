These are somes notes I made while reading [HyperBone](https://github.com/DarthTon/HyperBone) source code.

1. Check if HV is supported:
	- Look if CPU vendor is Intel by executing a CPUID(0).
	- Look if HVM is supported:
		- by executing a CPUID(1).VMX bit should be set.
		- Check if MSR FeatureControl.lock is not set, and set lock and enable_vmxon bit.
		- If lock bit is set, check enable_vmxon, if not set, return false, else, return true.  
2. Init internal structures 
	- Get ntoskrnl base address (ZwQuerySystemInformation.SystemModuleInformation).
	- Get SSDT base (KiServiceTable), loop over ntoskrl sections and look for specific pattern.
3. Allocate global data
	- Global data (GLOBAL_DATA)
	- MSR_Bitmap (PAGE_SIZE, NonPagedPoolNx)
	- EPT PageList double linked liste for each vCPU.
	- EPT pages: MmAllocateContiguousMemorySpecifyCache.PAGE_SIZE, MmProtectMdlSystemAddress(PAGE_READWRITE) for each vCPU.
4. Get physical memory regions
	- pBaseRange = MmGetPhysicalMemoryRanges
	- count number of ranges / number of pages.
	- get the apic physical page: _readmsr( MSR_APIC_BASE )
	- Fill the used PFN regions (PPHYSICAL_MEMORY_DESCRIPTOR)
5. Fill available CPU features
	- Read true MSRs, Secondary controls: EPT, VPID, VMFUNC, ExecOnlyEPT.
	- Fail if secondary controls, EPT/VPID or execute-only EPT are not supported.
6. Dispatch a DPC routine with cr3 as context param.
	- Store the hibernation state of the processor (KeSaveStateForHibernate): capture special registers and MSR.
	- Capture the host state (RtlCaptureContext)
	- If the VmxState is in transition, we set the VmxState to ON and we restore the context (VmRestoreContext)
	- If transition is OFF, capture the value of the PML4 for the SYSTEM process and attempt tp subvert the vCPU.
7. Virtualize the CPU
	- Initialize all the VMX-related MSRs by reading their value: secondary controls, true MSRs, VMFUNC
	- Allocate (MmAllocateContiguousMemory) space for VMXON, VMCS and VMMStack (KERNEL_STACK_SIZE=0x6000); protect them with PAGE_READWRITE
	- Attempt to enter root mode:
		- Ensure the the VMCS can fit into a single page !(pBasic->Fields.RegionSize > PAGE_SIZE)
		- Ensure that the VMCS is supported in writeback memory (pBasic->Fields.MemoryType != VMX_MEM_TYPE_WRITEBACK)
		- Ensure that true MSRs can be used for capabilities (pBasic->Fields.VmxCapabilityHint == 0)
		- Capture the revision ID for the VMXON and VMCS region
		- Update CR0 and CR4 with the must-be-zero and must-be-one requirements
		- Enable VMX Root Mode: __vmx_on (MmGetPhysicalAddress[VMXON])
		- Clear the state of the VMCS, setting it to Inactive: __vmx_vmclear (MmGetPhysicalAddress[VMCS])
		- Load the VMCS, setting its state to Active: __vmx_vmptrld (MmGetPhysicalAddress[VMCS])
	- Initialize the VMCS, both guest and host state:
		- We want to activate the MSR bitmap in order to keep them from being caught (UseMSRBitmaps=TRUE)
		- VPID caches must be invalidated on CR3 change (CR3LoadExiting=TRUE)
		- Load the MSR bitmap. Unlike other bitmaps, not having an MSR bitmap will trap all MSRs, so have to allocate an empty one.
	- Setup EPT:
		- Create Guest to Host page mappings
			- Allocate page for PTE table (PML4Ptr) 
			- Fill PML4 table accordingly to used physical regions (in a recursive way)
	- Enable EPT for CPU by enabling EnableEPT/EnableVPID in SecondaryControls and __invept( INV_ALL_CONTEXTS, &ctx );
8. VM-Exit handling (takes PCONTEXT) as param.
    - Raise IRQL to high level.
    - Read rflags, guest rip, guest rsp, exit reason, exit qualification, guest physical and linear address
		- Cpuid: execute __cpuidex and advance eip.
		- VmExitEptMisconfig: KeBugCheckEx()
		- VmExitEvent:
			- If INTERRUPT_NMI => VmxInjectEvent
			- If INTERRUPT_HARDWARE_EXCEPTION => VmxInjectEvent
			- If INTERRUPT_SOFTWARE_EXCEPTION:
				- If VECTOR_BREAKPOINT_EXCEPTION => VmxInjectEvent
				- else => VmxInjectEvent
			- Else => VmxInjectEvent
		- Triple Fault: KeBugCheckEx()
		- Everything else as : VmExitUnknown
		- Invd: execute __wbinvd and advance rip.
		- XSETBV: execute __xsetbv and advance rip.
		- RDTSC: execute __rdtsc and update GuestState->GpRegs and advance eip.
		- VMCALL: 
		- VmExitVMOP for all other VMX insutrctions: VmxInjectEvent
		- CrAccess: check qualification to know if it is a move to or move from
			- If move to, switch over control register and execute a __vmx_vmwrite for (GUEST_CR0 and CR0_READ_SHADOW)
			- If move to, and CR is CR3, if VPID supported, execute a _invvpid( INV_ALL_CONTEXTS ).
			- If move from,  switch over control register and execute a __vmx_vmread[regPtr].
		- MSR read: get the msr from guest->rcx.
			- MSR_LSTAR: OriginalLSTAR if OriginalLSTAR != 0 or __readmsr( MSR_LSTAR );
			- MSR_GS_BASE/MSR_FS_BASE/MSR_IA32_DEBUGCTL: read them from vmcs
			- MSR_IA32_FEATURE_CONTROL , set the lock bit and set EnableVmxon to 0.
			- MSR_IA32_VMX_XXX; read them from GuestState->Vcpu->MsrData
			- default: __readmsr( ecx );
			- Advance eip
		- MSR write: get the msr from guest->rcx.
			- MSR_LSTAR: if OriginalLSTAR == 0 => __writemsr( MSR_LSTAR );
			- MSR_GS_BASE/MSR_FS_BASE: write them to vmcs
			- MSR_IA32_DEBUGCTL:  write them to vmcs + __writemsr
			- MSR_IA32_VMX_XXX; read them from GuestState->Vcpu->MsrData
			- default: __writemsr( ecx );
			- Advance eip
		- INVALID_GUEST_STATE / MACHINE_CHECK / MSR_LOADING: KeBugCheckEx
		- MTF: