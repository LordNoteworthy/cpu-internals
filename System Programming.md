## Overview of The System Level Architecture
* System-level architecture consists of a set of registers, data structures, and instructions designed to support basic system-level operations such as memory management, interrupt and exception handling, task management, and control of multiple processors

<p align="center"> 
    <img src="https://i.imgur.com/O0QCFwt.png" width="700px" height="auto">
</p>

## Global and Local Descriptor Tables
* When operating in protected mode, all memory accesses pass through either the global descriptor table (GDT) or an optional local descriptor table (LDT).
* These tables contain entries called segment descriptors and can contain up to 8192 (213) 8-byte descriptors. 
* Segment descriptors (8 byte large) provide the base address of segments well as access rights, type, and usage information.
* Each segment descriptor has an associated segment selector.
* A segment selector (16-bits) provides the software that uses it with an index into the GDT or LDT (the offset of its associated segment descriptor), a global/local flag (determines whether the selector points to the GDT or the LDT), and access rights information (CPL).
* The linear address of the base of the GDT (as well as its length or limit) is contained in the GDT register (GDTR).
* The linear address of the LDT is contained in the LDT register (LDTR).
* GDTR (Global Descriptor Table Register) is a 48 bits register.
* GDTR: `upper 32bits` holds the base address where the GDT is stored and the `lower 16bits` holds the table limit which tell the size of the table.
* LDT is intended to used per process and switched when the kernel switch between process contexts. GDT is for use system wide.
* `LGDT`/`SGDT` and `LLDT`/`SLDT` are instructions which load/store data from the GDT/LDT register accordingly.
* The OS sets those tables.

## System Segments, Segment Descriptors, and Gates
* The architecture defines two system segments: the task-state segment (TSS) and the LDT.
* The architecture defines a set of special descriptors called gates: `call gates`, `interrupt gates`, `trap gates`, and `task gates`.
* These gates provide protected gateways to system procedures and handlers that may operate at a different privilege level than application programs and most procedures.

## Task-State Segments and Task Gates
* The `TSS` defines the state of the execution environment for a task. It includes the state of: 
    - General purpose registers, segment registers, the EFLAGS register, the EIP register.
    - And segment selectors with stack pointers for three stack segments (one stack for each privilege level). 
    - The TSS also includes the segment selector for the LDT associated with the task and the base address of the paging-structure hierarchy.
* All program execution in protected mode happens within the context of a task called the `current task`.
* In switching tasks, the processor performs the following actions:
    1. Stores the state of the current task in the current TSS.
    2. Loads the task register with the segment selector for the new task.
    3. Accesses the new TSS through a segment descriptor in the GDT.
    4. Loads the state of the new task from the new TSS into the general-purpose registers, the segment registers, the LDTR, control register CR3 (base address of the paging-structure hierarchy), the EFLAGS register, and the EIP register.
    5. Begins execution of the new task.
* A task can also be accessed through a task gate. A task gate is similar to a call gate, except that it provides access (through a segment selector) to a TSS rather than a code segment

## System Registers
* The `system flags` and `IOPL` field in the EFLAGS register control task and mode switching, interrupt handling, instruction tracing, and access rights.
* The `control registers CR0, CR2, CR3, and CR4` contain a variety of flags and data fields for controlling system level operations.
* The `debug registers` allow the setting of breakpoints for use in debugging programs and systems software. 
* The `GDTR, LDTR`, and `IDTR` registers contain the linear addresses and sizes (limits) of their respective tables.
* The `TR task register` contains the linear address and size of the TSS for the current task.
* Model-specific registers.

## Protected-Mode Memory Management
* At the system-architecture level in protected mode, the processor uses two stages of address translation to arrive at a physical address: logical-address translation through `segmentation` and linear address space through `paging`.
<p align="center"> 
    <img src="https://i.imgur.com/QpA3AS9.png" width="600px" height="auto">
</p

* Segmentation provides a mechanism for dividing adressable memory space into segments.
* Segmentation translates logical addresses to linear addresses in hardware by using table lookups.
* A logical address consists of a 16-bit segment selector and a 32-bit offset.
* To translate a logical address into a linear address, the processor does the following:
    1. Uses the offset in the segment selector to locate the segment descriptor for the segment in the GDT or LDT and reads it into the processor. (This step is needed only when a new segment selector is loaded into a segment register.)
    2. Examines the segment descriptor to check the access rights and range of the segment to insure that the segment is accessible and that the offset is within the limits of the segment.
    3. Adds the base address of the segment from the segment descriptor to the offset to form a linear address.

<p align="center"> 
    <img src="https://i.imgur.com/jh5Bp46.png" width="600px" height="auto">
</p

* Although a system can define thousands of segments, only 6 can be available for immediate use
* A segment selector is a 16-bit identifier for a segment. It does not point directly to the segment, but instead points to the segment descriptor that defines the segment.

<p align="center"> 
    <img src="https://i.imgur.com/JUOiXYX.png" width="600px" height="auto">
</p

* A segment have a visible part (segment selector) which can be read or written to by software running at any privilege level, and a hidden part which act like a cache so that segment descriptor info doesn't have to be looked up each time (only accessible by the hardware).
<p align="center"> 
    <img src="https://i.imgur.com/lxlmUtA.png" width="600px" height="auto">
</p>

* When a segment selector is loaded into the visible part of a segment register, the processor also loads the hidden part of the segment register with the base address, segment limit, and access control information from the segment descriptor pointed to by the segment selector. 
* Two kinds of load instructions are provided for loading the segment registers:
    1. Direct load instructions such as the MOV, POP, LDS, LES, LSS, LGS, and LFS instructions. These instructions explicitly reference the segment registers.
    2. Implied load instructions such as the far pointer versions of the CALL, JMP, and RET instructions, the SYSENTER and SYSEXIT instructions, and the IRET, INTn, INTO and INT3 instructions.

## Segment Descriptors
* A segment descriptor is a data structure in a GDT or LDT that provides the processor with the size and location of a segment, as well as access control and status information.
* Segment descriptors are typically created by compilers, linkers, loaders, or the OS or executive, but not application programs.

<p align="center"> 
    <img src="https://i.imgur.com/yjnRlez.png" width="600px" height="auto">
</p>

## System Descriptor Types
* When the S (descriptor type) flag in a segment descriptor is clear, the descriptor type is a system descriptor. The processor recognizes the following types of system descriptors:
    - Local descriptor-table (LDT) segment descriptor.
    - Task-state segment (TSS) descriptor.
    - Call-gate descriptor.
    - Interrupt-gate descriptor.
    - Trap-gate descriptor.
    - Task-gate descriptor.


## Segment Descriptor Tables
* Each system must have one GDT defined, which may be used for all programs and tasks in the system. Optionally, one or more LDTs can be defined. For example, an LDT can be defined for each separate task being run, or some or all tasks can share the same LDT.
<p align="center"> 
    <img src="https://i.imgur.com/qEnuApv.png" width="600px" height="auto">
</p>

* The base address of the GDT should be aligned on an eight-byte boundary to yield the best processor performance. 
* The limit value for the GDT is expressed in bytes. 
* As with segments, the limit value is added to the base address to get
the address of the last valid byte. A limit value of 0 results in exactly one valid byte. Because segment descriptors
are always 8 bytes long, the GDT limit should always be one less than an integral multiple of eight (that is, 8N – 1).
* The first descriptor in the GDT is not used by the processor. A segment selector to this `null descriptor` does not generate an exception when loaded into a data-segment register (DS, ES, FS, or GS), but it always generates a #GP exception when an attempt is made to access memory using the descriptor.
* The LDT is located in a system segment of the LDT type. The GDT must contain a segment descriptor for the LDT segment. If the system supports multiple LDTs, each must have a separate segment selector and segment descriptor in the GDT. The segment descriptor for an LDT can be located anywhere in the GDT.
* An LDT is accessed with its segment selector. To eliminate address translations when accessing the LDT, the segment selector, base linear address, limit, and access rights of the LDT are stored in the LDTR register