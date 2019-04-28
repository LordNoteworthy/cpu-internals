These are somes notes I made while reading [HyperBone](https://github.com/DarthTon/HyperBone) source code.

1. Check if HV is supported:
	- Look if CPU vendor is Intel by executing a CPUID(0).
	- Look if HVM is supported:
		- by executing a CPUID(1).VX bit should be set.
		- Check if MSR FeatureControl.lock is not set, and set lock and enable_vmxon bit.
		- If lock bit is set, check enable_vmxon, if not set, return false, else, return true.  
2. Init internal structures 
	- Get ntoskrnl base address (ZwQuerySystemInformation.SystemModuleInformation).
	- Get SSDT base (KiServiceTable), loop over ntoskrl sections and look for specific pattern.
3. Allocate global data
	- Global data (GLOBAL_DATA)
	- MSR_Bitmap (PAGE_SIZE, NonPagedPoolNx)
	- EPT pages: MmAllocateContiguousMemorySpecifyCache.PAGE_SIZE, MmProtectMdlSystemAddress(PAGE_READWRITE)
4. Get physical memory regions
	- pBaseRange = MmGetPhysicalMemoryRanges
	- count number of ranges / number of pages.
	- get the apic physical page: _readmsr( MSR_APIC_BASE )
	- Fill the used  PFN regions (PPHYSICAL_MEMORY_DESCRIPTOR)
5. Fill available CPU features
	- True MSRs, Secondary controls: EPT, VPID, VMFUNC, ExecOnlyEPT.
	- Fail if secondary controls, EPT/VPID or execute-only EPT are not supported.
6. Dispatch a DPC routine
	- Store the hibernation state of the processor (KeSaveStateForHibernate)
	- Capture the host state (RtlCaptureContext)
	- If the VmxState is in transition, we set the VmxState to ON and we restore the context (VmRestoreContext)
	- If transition is OFF, capture the value of the PML4 for the SYSTEM process and attempt tp subvert the vCPU.
7. Virtualize the CPU
	- Initialize all the VMX-related MSRs by reading their value: secondary controls, true MSRs, VMFUNC
	- Allocate (MmAllocateContiguousMemory) space for VMXON, VMCS and VMMStack; protect them with PAGE_READWRITE
	- Attempt to enter root mode:
		- Ensure the the VMCS can fit into a single page !(pBasic->Fields.RegionSize > PAGE_SIZE)
		- Ensure that the VMCS is supported in writeback memory (pBasic->Fields.MemoryType != VMX_MEM_TYPE_WRITEBACK)
		- Ensure that true MSRs can be used for capabilities (pBasic->Fields.VmxCapabilityHint == 0)
		- Capture the revision ID for the VMXON and VMCS region
		- Update CR0 and CR4 with the must-be-zero and must-be-one requirements
		- Enable VMX Root Mode: __vmx_on (MmGetPhysicalAddress[VMXON])
		- Clear the state of the VMCS, setting it to Inactive: __vmx_vmclear (MmGetPhysicalAddress[VMCS])
		- Load the VMCS, setting its state to Active: __vmx_vmptrld (MmGetPhysicalAddress[VMCS])
	- Initialize the VMCS, both guest and host state.
8. VM-Exit handling (takes PCONTEXT) as param.
    - Raise IRQL to high level.
    - Read rflags, guest rip, guest rsp, exit reason, exit qualification, guest physical and linear address
    - VmExitCPUID handler: execute __cpuidex and advance eip.
    - VmExitCR handler: check qualification to know if it is a move to or move from
        - If move to, switch over control register and execute a __vmx_vmwrite for (GUEST_CR0 and CR0_READ_SHADOW)
        - If move to, and CR is CR3, if VPID supported, execute a _invvpid( INV_ALL_CONTEXTS ).
        - If move from,  switch over control register and execute a __vmx_vmread[regPtr].
	- VmExitEptMisconfig: KeBugCheckEx()