# cpu-internals
Intel / AMD CPU Internals

#### Virtual Machine Architecture:
* Virtual Machine Monitor (VMM) aka (Hypervisor) act as host and has full control of the processor(s) and the hardware (physical memory, interrupt management and  I/O).
* It provides the guest or the (Virtual Machine) with an abstraction of a virtual processor, allowing it to think it is execute directly on the logical processor.

#### Introduction To VMX Operation:
* Processor support for virtualization is provided by a form of processor operation called VMX (Virtual Machine eXtensions).
* There are two kind of VMX operations:
	- VMX root operation (VMM will generally run here).
	- VMX non-root operation (VM will generally run here).
- The main differences between these two modes is that in root mode, a new set of new instructions (VMX instructions) is available and that the values that can be loaded into certain control registers are limited.
