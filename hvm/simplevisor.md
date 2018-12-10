These are somes notes I made while reading [SimpleVisor](https://github.com/ionescu007/SimpleVisor) source code.

* Register power callback routines dealing with hibernate and sleep states: ```\\Callback\\PowerState```.
*  Attempt to enter VMX root mode on all LP by broadcasting a DPC interrupt which will execute the callback routine in parallel on the LPs: ```KeGenericCallDpc(Routine, Context)```
	- Check if the HypervisorPresent bit is set: ```CPUID[1].ECX[31] == 1```.
	- Check if the Feature Control MSR is locked: ```readmsr[0x03A].lock```.
	- The Feature Control MSR is locked-in (valid). Is VMX enabled in normal operation mode (outside smx): ```IA32_FEATURE_CONTROL_MSR_ENABLE_VMXON_OUTSIDE_SMX]```
	- Allocate the per-VP data ```PSHV_VP_DATA```
	- Capture the value of the PML4 for the SYSTEM process : ```Data->SystemDirectoryTableBase = Context->Cr3;```
	- Read the special control registers for this processor.
	- Capture the entire register state: ```RtlCaptureContext```.
	- If the AC bit is not set in EFLAGS, it means that we have not yet launched the VM: ```__readeflags & EFLAGS_ALIGN_CHECK```.
	- Initialize VMX on this VP:
		- Initialize all the VMX-related MSRs by reading their value.
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
		- Initialize the VMCS, both guest and host state.
			- Begin by setting the link pointer to the required value for 4KB VMCS.
			- Enable EPT features if supported: configure the EPTP and VPID.
			- Load the MSR bitmap
			- Enable support for RDTSCP and XSAVES/XRESTORES
			- Make sure to enter and exit in x64 mode
			- Load the CS/DS/SS/ES/FS/GS Segment
			- Load the TR/LDT/IDT/GDT 
			- Load the CR0/CR3/CR4
			- Load debug MSR and register (DR7)
			- Load the guest stack, instruction pointer, and rflags
			- Load the hypervisor entrypoint and stack
		- Launch the VMCS: ```_vmx_vmlaunch```
* When a VM Exit happen:
	- Read RIP/RSP/RFLAGS, and the exit reason.
	- Call the generic handler