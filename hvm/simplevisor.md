These are somes notes I made while reading [SimpleVisor](https://github.com/ionescu007/SimpleVisor) source code.

1. Register power callback routines dealing with hibernate and sleep states: ```\\Callback\\PowerState```.
2. Attempt to enter VMX root mode on all LP by broadcasting a DPC interrupt which will execute the callback routine in parallel on the LPs: ```KeGenericCallDpc(Routine, Context)```
   - Check if the HypervisorPresent bit is set: ```CPUID[1].ECX[31] == 1```.
   - Check if the Feature Control MSR is locked: ```readmsr[0x03A].lock```.
   - The Feature Control MSR is locked-in (valid). Is VMX enabled in normal operation mode (outside smx): ```IA32_FEATURE_CONTROL_MSR_ENABLE_VMXON_OUTSIDE_SMX]```
   - Allocate the per-VP data ```PSHV_VP_DATA```
   - Capture the value of the PML4 for the SYSTEM process : ```Data->SystemDirectoryTableBase = Context->Cr3;```
   - Read the special control registers for this processor.
   - Capture the entire register state: ```RtlCaptureContext```.
   - If the AC bit is not set in EFLAGS, it means that we have not yet launched the VM: ```__readeflags & EFLAGS_ALIGN_CHECK```.
   - Initialize VMX on this VP:
   	- Initialize all the VMX-related MSRs by reading their value: ```__readmsr(MSR_IA32_VMX_BASIC + i)```
	- Initialize all the MTRR-related MSRs by reading their value and build range structures to describe their settings.
	- Initialize the EPT structures.
	- Attempt to enter VMX root mode on this processor:
	  - Ensure the the VMCS can fit into a single page
	  - Ensure that the VMCS is supported in writeback memory
	  - Ensure that true MSRs can be used for capabilities
	  - Ensure that EPT is available with the needed features SimpleVisor uses and enable EPT if these features are supported.
	  - Capture the revision ID for the VMXON and VMCS region
	  - Store the physical addresses of all per-LP structures allocated
	  - Update CR0/CR4 with the must-be-zero and must-be-one requirements
	  - Enable VMX Root Mode: ```__vmx_on```
	  - Clear the state of the VMCS, setting it to Inactive: ```__vmx_vmclear```
	  - Load the VMCS, setting its state to Active: ```__vmx_vmptrld```
	- Setup the VMCS.
	  - Begin by setting the link pointer to the required value for 4KB VMCS.
	  - Enable EPT features if supported: configure the EPTP and VPID.
	  - Load the MSR bitmap, we allocate an empty one to avoid trapping on all MSRs.
	  - Enable support for RDTSCP and XSAVES/XRESTORES (Win10 makes use of them)
	  - Also enable EPT support, for additional performance and ability to trap memory access efficiently: `SECONDARY_EXEC_ENABLE_INVPCID`.
	  - Enable no pin-based options ourselves
	  - In order for our choice of supporting RDTSCP and XSAVE/RESTORES above to actually mean something, we have to request secondary controls. We also want to activate the MSR bitmap in order to keep them from being caught.
	  - Make sure to enter us in x64 mode at all times.
	  - As we exit back into the guest, make sure to exist in x64 mode as well.
	  - Load the CS/DS/SS/ES/FS/GS Segment
	  - Load the TR/LDT/IDT/GDT 
	  - Load the CR0/CR3/CR4
	  - Load debug MSR and register (DR7)
	  - Load the guest stack, instruction pointer, and rflags
  		```
		__vmx_vmwrite(GUEST_RSP, (uintptr_t)VpData->ShvStackLimit + KERNEL_STACK_SIZE - sizeof(CONTEXT));
  		__vmx_vmwrite(GUEST_RIP, (uintptr_t)ShvVpRestoreAfterLaunch);
		``` 
	  - Load the hypervisor entrypoint and stack
	  	```
	    __vmx_vmwrite(HOST_RSP, (uintptr_t)VpData->ShvStackLimit + KERNEL_STACK_SIZE - sizeof(CONTEXT));
   	 	__vmx_vmwrite(HOST_RIP, (uintptr_t)ShvVmxEntry);
		```
	- Launch the VMCS: ```_vmx_vmlaunch```
- When a VM Exit happen:
	- Read RIP/RSP/RFLAGS, and the exit reason.
	- Call the generic handler with the guest context as argument.
      - Handles EXIT_REASON_CPUID
        - Check for the magic CPUID sequence to unload
        - Check if CPUID == 1h, which is the features request => Set the Hypervisor Present-bit in RCX
        - Check if CPUID == HypervisorVendor => Return our interface identifier
        - Otherwise execute a cpuid and return values
      - Handles EXIT_REASON_INVD
        - executes a __wbinvd
      - Handles EXIT_REASON_XSETBV
        - Simply issue the XSETBV instruction on the native logical processor. 
      - Handles EXIT_REASON_VMXON
        -  Set the CF flag, which is how VMX instructions indicate failure 
  - Move the instruction pointer to the next instruction after the one that caused the exit. Since we are not doing any special handling or changing of execution, this can be done for any exit reason.
	- 