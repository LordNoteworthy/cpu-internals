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
	 ![VMX transitions](https://raw.githubusercontent.com/LordNoteworthy/cpu-internals/master/figures/Interaction%20of%20a%20Virtual-Machine%20Monitor%20and%20Guests.png)

#### Virtual-machine Control Structure
* VMCS is a data structure which control the behavior of processor in VMX non-root mode and control VMX transitions.
* Access to the VMCS is managed through a component of processor state called the VMCS pointer (one per logical processor).
* The VMCS pointer (64-bit) contain the physical address of the the VMCS and should be aligned to a 4-KB boundary.
* The VMCS pointer is read and written using the instructions VMPTRST and VMPTRLD. 
* The VMM configures a VMCS using the VMREAD, VMWRITE, and VMCLEAR instructions.
* A VMM can use a different VMCS for each VM that it supports.
* For a VM with multiple Logical Processors(LPs) or Virtual CPUs (vCPUs), the VMM can use a different VMCS for each vCPU.