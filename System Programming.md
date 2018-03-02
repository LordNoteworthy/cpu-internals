## Overview of The System Level Architecture
- System-level architecture consists of a set of registers, data structures, and instructions designed to support basic system-level operations such as memory management, interrupt and exception handling, task management, and control of multiple processors

<p align="center"> 
    <img src="https://i.imgur.com/O0QCFwt.png" width="700px" height="auto">
</p>

## Global and Local Descriptor Tables
- When operating in protected mode, all memory accesses pass through either the global descriptor table (GDT) or an optional local descriptor table (LDT).
- These tables contain entries called segment descriptors.
- Segment descriptors (8 byte large) provide the base address of segments well as access rights, type, and usage information.
- Each segment descriptor has an associated segment selector.
- A segment selector (16-bits) provides the software that uses it with an index into the GDT or LDT (the offset of its associated segment descriptor), a global/local flag (determines whether the selector points to the GDT or the LDT), and access rights information (CPL).
- The linear address of the base of the GDT is contained in the GDT register (GDTR).
- The linear address of the LDT is contained in the LDT register (LDTR).
- GDTR (Global Descriptor Table Register) is a 48 bits register.
- GDTR: upper 32bits holds the base address where the GDT is stored and the lower 16bits holds the table limit which tell the size of the table.
- LDT is intended to used per process and switched when the kernel switch between process contexts. GDT is for use system wide.
- `LGDT`/`SGDT` and `LLDT`/`SLDT` are instructions which load/store data from the GDT/LDT register accordingly.
- The OS sets those tables.

## Protected-Mode Memory Management
- At the system-architecture level in protected mode, the processor uses two stages of address translation to arrive at a physical address: logical-address translation through `segmentation` and linear address space through `paging`.
<p align="center"> 
    <img src="https://i.imgur.com/QpA3AS9.png" width="600px" height="auto">
</p

- Segmentation provides a mechanism for dividing adressable memory space into segments.
- Segmentation translates logical addresses to linear addresses in hardware by using table lookups.
- A logical address consists of a 16-bit segment selector and a 32-bit offset.
- To translate a logical address into a linear address, the processor does the following:
    1. Uses the offset in the segment selector to locate the segment descriptor for the segment in the GDT or LDT and reads it into the processor. (This step is needed only when a new segment selector is loaded into a segment register.)
    2. Examines the segment descriptor to check the access rights and range of the segment to insure that the segment is accessible and that the offset is within the limits of the segment.
    3. Adds the base address of the segment from the segment descriptor to the offset to form a linear address.

<p align="center"> 
    <img src="https://i.imgur.com/jh5Bp46.png" width="600px" height="auto">
</p


- Although a system can define thousands of segments, only 6 can be available for immediate use
- A segment selector is a 16-bit identifier for a segment. It does not point directly to the segment, but instead points to the segment descriptor that defines the segment.

<p align="center"> 
    <img src="https://i.imgur.com/JUOiXYX.png" width="600px" height="auto">
</p

- A segment have a visible part (segment selector) which can be read or written to by software running at any privilege level, and a hidden part which act like a cache so that segment descriptor info doesn't have to be looked up each time (only accessible by the hardware).
<p align="center"> 
    <img src="https://i.imgur.com/lxlmUtA.png" width="600px" height="auto">
</p

- When a segment selector is loaded into the visible part of a segment register, the processor also loads the hidden part of the segment register with the base address, segment limit, and access control information from the segment descriptor pointed to by the segment selector. 
- Two kinds of load instructions are provided for loading the segment registers:
    1. Direct load instructions such as the MOV, POP, LDS, LES, LSS, LGS, and LFS instructions. These instructions explicitly reference the segment registers.
    2. Implied load instructions such as the far pointer versions of the CALL, JMP, and RET instructions, the SYSENTER and SYSEXIT instructions, and the IRET, INTn, INTO and INT3 instructions.

When the S (descriptor type) flag in a segment descriptor is clear, the descriptor type is a system descriptor. The processor recognizes the following types of system descriptors:
- Local descriptor-table (LDT) segment descriptor.
- Task-state segment (TSS) descriptor.
- Call-gate descriptor.
- Interrupt-gate descriptor.
- Trap-gate descriptor.
- Task-gate descriptor.