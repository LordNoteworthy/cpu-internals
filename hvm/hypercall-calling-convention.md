### Xen Hypercall Interface in x86 64-bit mode
* RCX – Call Code
* RDI – Input Parameter 1
* RSI – Input Parameter 2
* RDX – Input Parameter 3
* R10 – Input Parameter 4
* R8 - Input Parameter 5
* Up to 5 input parameters can be used by hypercall handler.
* One input parameter may be a Guest Virtual Address pointing to a hypercall-specific data structure

### Hyper-V Hypercall Interface in x86 64-bit mode
* Memory-based calling convention
    - RCX - Hypercall Input Value*
    - RDX - Input Parameters GPA
    - R8 - Output Parameters GPA
* Register-based calling convention (Fast Hypercall)
    - RCX - Hypercall Input Value*
    - RDX - Input Parameter
    - R8 - Input Parameter
    - XMM0-XMM5 - Input Parameters (XMM Fast Hypercall if uses more than two input parameters)
* *Hypercall Input Value includes call code, fast hypercall bit, variable header size, rep count & start index.
* Hyper-V uses GPA when sending arguments with hypercalls. Smarter than what Xen are doing.

### KVM Hypercall Interface in x86 64-bit mode
* KVM Hypercalls have a three-byte sequence of either the vmcall or the vmmcall instruction.
* The hypervisor can replace it with instructions that are guaranteed to be supported.
* Up to four arguments may be passed in rbx, rcx, rdx, and rsi respectively.
* The hypercall number should be placed in rax and the return value will be placed in rax.
* No other registers will be clobbered unless explicitly stated by the particular hypercall.