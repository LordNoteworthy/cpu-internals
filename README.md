# cpu-internals

### Virtual Machine Architecture:
* Virtual Machine Monitor (VMM) aka (Hypervisor) act as host and has full control of the processor(s) and the hardware (physical memory, interrupt management and  I/O).
* It provides the guest or the (Virtual Machine) with an abstraction of a virtual processor, allowing it to think it is execute directly on the logical processor.

### Introduction To VMX Operation:
* Processor support for virtualization is provided by a form of processor operation called VMX (Virtual Machine eXtensions).
* There are two kind of VMX operations:
	- VMX root operation (VMM will generally run here).
	- VMX non-root operation (VM will generally run here).
- The main differences between these two modes is that in root mode, a new set of new instructions (VMX instructions) is available and that the values that can be loaded into certain control registers are limited.

### Life Cycle of VMM Software:
* The following items summarize the life cycle of a VMM and its guest software as well as the interactions between them:
	- Software enters VMX operation by executing a VMXON instruction.
	- Using VM entries, a VMM can then turn guests into VMs (one at a time). The VMM effects a VM entry using instructions VMLAUNCH and VMRESUME; it regains control using VM exits.
	- VM exits transfer control to an entry point specified by the VMM. The VMM can take action appropriate to the cause of the VM exit and can then return to the VM using a VM entry.
	- Eventually, the VMM may decide to shut itself down and leave VMX operation. It does so by executing the VMXOFF instruction.

<p align="center">
  <img src="https://raw.githubusercontent.com/LordNoteworthy/cpu-internals/master/figures/Interaction%20of%20a%20Virtual-Machine%20Monitor%20and%20Guests.png" alt="VMX transition"/>
</p>

### Virtual-machine Control Structure
* VMCS is a data structure which control the behavior of processor in VMX non-root mode and control VMX transitions.
* Access to the VMCS is managed through a component of processor state called the VMCS pointer (one per logical processor).
* The VMCS pointer (64-bit) contain the physical address of the the VMCS region and should be aligned to a 4-KB boundary.
* The VMCS pointer is read and written using the instructions VMPTRST and VMPTRLD. 
* The VMM configures a VMCS using the VMREAD, VMWRITE, and VMCLEAR instructions.
* A VMM can use a different VMCS for each VM that it supports.
* For a VM with multiple Logical Processors(LPs) or Virtual CPUs (vCPUs), the VMM can use a different VMCS for each vCPU.

### Discovering Support For VMX:
* Before system software enters into VMX operation, it must discover the presence of VMX support in the processor.
* This is achieved by executing cpuid (1) and checking if ECX.VMX (bit 5) = 1, them VMX operations is supported.
* The VMX architecture is designed to be extensible so that future processors in VMX operation can support additional features not present in first-generation implementations of the VMX architecture. 
* The availability of extensible VMX features is reported to software using a set of VMX capability MSRs.

### Enabling And Entering VMX Operation:
* Before system software can enter VMX operation, it enables VMX by setting CR4.VMXE[bit 13] = 1. 
* VMXON causes an invalid-opcode exception (#UD) if executed with CR4.VMXE = 0.
* Once in VMX operation, it is not possible to clear CR4.VMXE. CR4.VMXE can be cleared outside of VMX operation after executing of VMXOFF.
* VMXON is also controlled by the IA32_FEATURE_CONTROL MSR (MSR address 3AH). The relevant bits of the MSR are:
	- Bit 0 is the lock bit: If this bit is 0, VMXON causes a #GP exception. If the lock bit is 1, WRMSR to this MSR causes a #GP exception. 
	- Bit 1 enables VMXON in SMX operation: If this bit is 0, execution of VMXON in SMX operation causes a #GP exception.
	- Bit 2 enables VMXON outside SMX operation. If this bit is 0, execution of VMXON outside SMX operation causes a #GP exception.
* To enable VMX support in a platform, BIOS must set bit 1, bit 2, or both , as well as the lock bit.

### Restrictions On VMX Operation:
* VMX operation places restrictions on processor operation:
    - In VMX operation, processors may fix certain bits in CR0 and CR4 to specific values and not support other values. VMXON fails if any of these bits contains an unsupported value.
    - Any attempt to set one of these bits to an unsupported value while in VMX operation using any of the CLTS, LMSW, or MOV CR instructions causes a #GP exception. VM entry or VM exit cannot set any of these bits to an unsupported value.
* Software should consult the VMX capability:
    - MSRs IA32_VMX_CR0_FIXED0 and IA32_VMX_CR0_FIXED1 to determine how bits are fixed for CR0.
    - MSRs IA32_VMX_CR4_FIXED0 and IA32_VMX_CR4_FIXED1 to determine how bits are fixed for CR4.
* The first processors to support VMX operation require that the following bits be 1 in VMX operation:
    * CR0.PE, CR0.NE, CR0.PG, and CR4.VMXE.
* The restrictions on CR0.PE and CR0.PG imply that VMX operation is supported only in paged protected mode. Therefore, guest software cannot be run in unpaged protected mode or in real-address mode.
* Later processors support a VM-execution control called “unrestricted guest”. If this control is 1, CR0.PE and CR0.PG may be 0 in VMX non-root operation (even if the capability MSR IA32_VMX_CR0_FIXED0 reports otherwise). Such processors allow guest software to run in unpaged protected mode or in real-address mode.

### Overview:
* At any given time, at most one of the active VMCSs is the current VMCS. 
* The VMLAUNCH, VMREAD, VMRESUME, and VMWRITE instructions operate only on the current VMCS.
* The VMCS link pointer field in the current VMCS is itself the address of a VMCS.
* If VM entry is performed successfully with the 1-setting of the “VMCS shadowing” VM-execution control, the VMCS referenced by the VMCS link pointer field becomes active on the logical processor. The identity of the current VMCS does not change.
* The launch state of a VMCS determines which VM-entry instruction should be used with that VMCS.
* Executing a VMPTRLD makes the VMCS is both active and current on the LP. 
* Executing a VMCLEAR makes the VMCS neither active nor current on the LP.
* Executing a VMLAUNCH instruction requires a VMCS whose launch state is “clear” and makes the launch state "launched".
* Executing a VMRESUME instruction requires a VMCS whose launch state is “launched”.

<p align="center">
  <img src="https://raw.githubusercontent.com/LordNoteworthy/cpu-internals/master/figures/States%20of%20VMCS%20X.png" alt="States of VMCS X"/>
</p>

### Format Of The VMCS Region:
* A VMCS region comprises up tp 4KB. To determine the exact size of the VMCS region, check VMX capability MSR IA32_VMX_BASIC.
* The format of a VMCS is structured as below:

| Byte Offset | Contents |
| --- | --- |
| 0 | Bits 30:0: VMCS revision identifier, Bit 31: shadow-VMCS indicator |
| 4 | VMX-abort indicator |
| 8 | VMCS data (implementation-specific format) |

* The first 4 bytes of the VMCS region contain the VMCS revision identifier at bits 30:0. This helps to avoid using a VMCS region formatted for one processor on a processor that uses a different format.
* Software can discover the VMCS revision identifier that a processor uses by reading the VMX capability MSR IA32_VMX_BASIC.
* VMPTRLD fails if its operand references a VMCS region whose VMCS revision identifier differs from that used by the processor. 
* Bit 31 of this 4-byte region indicates whether the VMCS is a shadow VMCS.
* Software can discover support for this setting by reading the VMX capability MSR IA32_VMX_PROCBASED_CTLS2
* VMPTRLD fails if the shadow-VMCS indicator is set and the processor does not support the 1-setting of the “VMCS shadowing” VM-execution control.
* The next 4 bytes of the VMCS region are used for the VMX-abort indicator. The contents of these bits do not control processor operation in any way. A logical processor writes a non-zero value into these bits if a VMX abort occurs. Software may also write into this field.
* The remainder of the VMCS region is used for VMCS data (those parts of the VMCS that control VMX non-root operation and the VMX transitions). The format of these data is implementation-specific.
* To ensure proper behavior in VMX operation, software should maintain the VMCS region and related structures in writeback cacheable memory, check the VMX capability MSR IA32_VMX_BASIC.

### Organization Of VMCS Data:
The VMCS data are organized into six logical groups:
* Guest-state area: Processor state is saved into the guest-state area on VM exits and loaded from there on VM entries.
* Host-state area: Processor state is loaded from the host-state area on VM exits.
* VM-execution control fields: These fields control processor behavior in VMX non-root operation. They determine in part the causes of VM exits.
* VM-exit control fields: These fields control VM exits.
* VM-entry control fields: These fields control VM entries.
* VM-exit information fields: These fields receive information on VM exits and describe the cause and the nature of VM exits. On some processors, these fields are read-only.

#### VMCS Layout:
* The VMCS layout is available in a form of table here: [VMCS Layout.pdf](../master/pdf/VMCS.pdf)

#### Instructions That Cause VM Exits Unconditionally
* The following instructions cause VM exits when they are executed in VMX non-root operation: 
	- CPUID, GETSEC, INVD, and XSETBV. 
	- INVEPT, INVVPID, VMCALL, VMCLEAR, VMLAUNCH, VMPTRLD, VMPTRST, VMRESUME, VMXOFF, and VMXON.

#### Instructions That Cause VM Exits Conditionally
* Instructions cause VM exits in VMX non-root operation depending on the setting of the VM-execution controls.
	- CLTS
	- ENCLS
	- HLT
	- IN, INS/INSB/INSW/INSD, OUT, OUTS/OUTSB/OUTSW/OUTSD.
	- INVLPG
	- INVPCID
	- LGDT, LIDT, LLDT, LTR, SGDT, SIDT, SLDT, STR
	- LMSW
	- MONITOR
	- MOV from CR3/CR8, MOV to CR0/1/3/4/8
	- MOV DR
	- MWAIT
	- PAUSE
	- RDMSR, WRMSR
	- RDPMC
	- RDRAND, RDSEED
	- RDTSC, RDTSCP
	- RSM
	- VMREAD, VMWRITE
	- WBINVD
	- XRSTORS, XSAVES


