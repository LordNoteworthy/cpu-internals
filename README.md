# cpu-internals

#### Virtual Machine Architecture:
* Virtual Machine Monitor (VMM) aka (Hypervisor) act as host and has full control of the processor(s) and the hardware (physical memory, interrupt management and  I/O).
* It provides the guest or the (Virtual Machine) with an abstraction of a virtual processor, allowing it to think it is execute directly on the logical processor.

#### Introduction To VMX Operation:
* Processor support for virtualization is provided by a form of processor operation called VMX (Virtual Machine eXtensions).
* There are two kind of VMX operations:
	- VMX root operation (VMM will generally run here).
	- VMX non-root operation (VM will generally run here).
- The main differences between these two modes is that in root mode, a new set of new instructions (VMX instructions) is available and that the values that can be loaded into certain control registers are limited.

#### Life Cycle of VMM Software:
* The following items summarize the life cycle of a VMM and its guest software as well as the interactions between them:
	- Software enters VMX operation by executing a VMXON instruction.
	- Using VM entries, a VMM can then turn guests into VMs (one at a time). The VMM effects a VM entry using instructions VMLAUNCH and VMRESUME; it regains control using VM exits.
	- VM exits transfer control to an entry point specified by the VMM. The VMM can take action appropriate to the cause of the VM exit and can then return to the VM using a VM entry.
	- Eventually, the VMM may decide to shut itself down and leave VMX operation. It does so by executing the VMXOFF instruction.

<p align="center">
  <img src="https://raw.githubusercontent.com/LordNoteworthy/cpu-internals/master/figures/Interaction%20of%20a%20Virtual-Machine%20Monitor%20and%20Guests.png" alt="VMX transition"/>
</p>

#### Virtual-machine Control Structure
* VMCS is a data structure which control the behavior of processor in VMX non-root mode and control VMX transitions.
* Access to the VMCS is managed through a component of processor state called the VMCS pointer (one per logical processor).
* The VMCS pointer (64-bit) contain the physical address of the the VMCS region and should be aligned to a 4-KB boundary.
* The VMCS pointer is read and written using the instructions VMPTRST and VMPTRLD. 
* The VMM configures a VMCS using the VMREAD, VMWRITE, and VMCLEAR instructions.
* A VMM can use a different VMCS for each VM that it supports.
* For a VM with multiple Logical Processors(LPs) or Virtual CPUs (vCPUs), the VMM can use a different VMCS for each vCPU.

#### Discovering Support For VMX:
* Before system software enters into VMX operation, it must discover the presence of VMX support in the processor.
* This is achieved by executing cpuid (1) and checking if ECX.VMX (bit 5) = 1, them VMX operations is supported.
* The VMX architecture is designed to be extensible so that future processors in VMX operation can support additional features not present in first-generation implementations of the VMX architecture. 
* The availability of extensible VMX features is reported to software using a set of VMX capability MSRs.

#### Enabling And Entering VMX Operation:
* Before system software can enter VMX operation, it enables VMX by setting CR4.VMXE[bit 13] = 1. 
* VMXON causes an invalid-opcode exception (#UD) if executed with CR4.VMXE = 0.
* Once in VMX operation, it is not possible to clear CR4.VMXE. CR4.VMXE can be cleared outside of VMX operation after executing of VMXOFF.
* VMXON is also controlled by the IA32_FEATURE_CONTROL MSR (MSR address 3AH). The relevant bits of the MSR are:
	- Bit 0 is the lock bit: If this bit is 0, VMXON causes a #GP exception. If the lock bit is 1, WRMSR to this MSR causes a #GP exception. 
	- Bit 1 enables VMXON in SMX operation: If this bit is 0, execution of VMXON in SMX operation causes a #GP exception.
	- Bit 2 enables VMXON outside SMX operation. If this bit is 0, execution of VMXON outside SMX operation causes a #GP exception.
* To enable VMX support in a platform, BIOS must set bit 1, bit 2, or both , as well as the lock bit.


#### Overview:
* At any given time, at most one of the active VMCSs is the current VMCS. 
* The VMLAUNCH, VMREAD, VMRESUME, and VMWRITE instructions operate only on the current VMCS.
* The VMCS link pointer field in the current VMCS is itself the address of a VMCS.
* If VM entry is performed successfully with the 1-setting of the “VMCS shadowing” VM-execution control, the VMCS referenced by the VMCS link pointer field becomes active on the logical processor. The identity of the current VMCS does not change.
* The launch state of a VMCS determines which VM-entry instruction should be used with that VMCS.
* Executing a VMPTRLD makes the VMCS is both active and current on the LP. 
* Executing a VMCLEAR makes the VMCS neither active nor current on the LP.
* Executing a VMLAUNCH instruction requires a VMCS whose launch state is “clear” and makes the launch state "launched".
* Executing a VMRESUME instruction requires a VMCS whose launch state is “launched”.



