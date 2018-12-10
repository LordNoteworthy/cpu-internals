## CPU Modes:

- Protected mode: native state of the processor. Includes (virtual-8086) feature to execute `real-mode` 8086 software in protected, multi-tasking env.
- Real-address mode: programming env for 8086 + ability to switch to protected more or SMM mode. The processor is placed in real mode after power-up or reset.
- SMM ( System Management Mode): implements platform-specific functions such as power management and system security. It enters SMM modes when SMI is received from the APIC or when the external SMM interrupt pin (SMI#) is activated.

Intel 64 architecture adds the IA-32e mode. IA-32e mode has two sub-modes. These are:
- Compatibility mode: permits most legacy 16-bit and 32-bit apps to run without re-compilation under a 64-bit OS.
- Long mode: This mode enables a 64-bit operating system to run apps written to access 64-bit linear address space. 
- In 64-bits mode: GPR and SIMD registers extends from 8 to 16 + GPR are widened to 64 bits.

## Basic Execution Environment:
Any program or task running on an IA-32 processor is given a set of resources for executing instructions and for storing code, data, and state information.

<p align="center"> 
    <img src="https://i.imgur.com/i9Djo8N.png" width="700px" height="auto">
</p>

<p align="center"> 
    <img src="https://i.imgur.com/5V6dS2F.png" width="700px" height="auto">
</p>

## IA-32 Memory Models:
Programs do not directly address physical memory. Instead, they access memory using one of three memory models: `flat`, `segmented`, or `real` address mode:
- Flat memory model: Memory appears to a program as a single, continuous address space (`Code`, `data`, and `stacks` are all contained in this address space.) ->  adr from 0 to 2^32 - 1.
- Segmented memory mode: Memory appears to a program as a group of independent address spaces called segments. Code, data, and stacks are typically contained in separate segments.
    - To address a byte in a segment, a program issues a logical address (far pointers) = segment selector + offset.
    - Programs running on an IA-32 processor can address up to 16,383 segments (2^14 - 1).
    - Internally, all the segments that are defined for a system are mapped into the processor’s linear address space. To access a memory location, the processor thus translates each logical address into a linear address. This translation is transparent to the application program.
    - The primary reason for using segmented memory is to increase the reliability of programs and systems. For example, placing a program’s stack in a separate segment prevents the stack from growing into the code or data space and overwriting instructions or data, respectively.
- Real-address mode memory mode: this is the memory model for the Intel 8086 processor.
    - Linear address space for the program and the operating system/executive consists of an array of segments of up to 64 KBytes in size each.
    - The maximum size of the linear address space in real-address mode is 220 bytes.

 <p align="center"> 
    <img src="https://i.imgur.com/EfpFmVB.png" width="700px" height="auto">
</p>

## Paging and Virtual Memory
- When using direct mapping (paging disabled), each linear address has a one-to-one correspondence with a physical address.
- When paging is enabled, linear address space is divided into pages which are mapped to virtual memory. The pages of virtual memory are then mapped as needed into physical memory.
- Paging is completely transparent to an app, all that the application sees is linear address space.
- In addition, IA-32 architecture’s paging mechanism includes extensions that support:
    - `Physical Address Extensions (PAE)` to address physical address space greater than 4 GBytes.
    - `Page Size Extensions (PSE)` to map linear address to physical address in 4-MBytes pages


## Modes of Operation vs. Memory Model
- When writing code for an IA-32 or Intel 64 processor, a programmer needs to know the operating mode the processor is going to be in when executing the code and the memory model being used.
    - Protected mode: the processor can use any of the memory models.
    - Real mode: the processor can only supports the real-address mode memory model.
    - SMM mode: the processor switches to a separate address space, called the system management RAM (SMRAM). The memory model used to address bytes in this address space is similar to the real-address mode model.
    - Compatibility mode: Software that needs to run in compatibility mode should observe the same memory model as those targeted to run in 32-bit protected mode. The effect of segmentation is the same as it is in - 32-bit protected mode semantics.
    - 64-bit mode — Segmentation is generally (but not completely) disabled, creating a flat 64-bit linear-address space. Specifically, the processor treats the segment base of CS, DS, ES, and SS as zero in 64-bit mode (this makes a linear address equal an effective address). Segmented and real address modes are not available in 64-bit mode.

## General Purpose Registers:
- EAX — Accumulator for operands and results data
- EBX — Pointer to data in the DS segment
- ECX — Counter for string and loop operations
- EDX — I/O pointer
- ESI — Pointer to data in the segment pointed to by the DS register; source pointer for string operations
- EDI — Pointer to data (or destination) in the segment pointed to by the ES register; destination pointer for string operations
- ESP — Stack pointer (in the SS segment)
- EBP — Pointer to data on the stack (in the SS segment)

## General-Purpose Registers in 64-Bit Mode
- 16 general purpose registers and the default operand size is 32 bits.
- If a 32-bit operand size is specified: EAX,EBX, ECX, EDX, EDI, ESI, EBP, ESP, R8D - R15D are available.
- If a 64-bit operand size is specified: RAX, RBX, RCX, RDX, RDI, RSI, RBP, RSP, R8-R15 are available. R8D-R15D/R8-R15 represent eight new general-purpose registers.
- All of these registers can be accessed at the byte, word, dword, and qword level. REX prefixes are used to generate 64-bit operand sizes or to reference registers R8-R15.
- Registers only available in 64-bit mode (R8-R15 and XMM8-XMM15) are preserved across transitions from 64-bit mode into compatibility mode then back into 64-bit mode.
- However, values of R8-R15 and XMM8-XMM15 are undefined after transitions from 64-bit mode through compatibility mode to legacy or real mode and then back through compatibility mode to 64-bit mode

## Segment Registers
- The segment registers (CS, DS, SS, ES, FS, and GS) hold 16-bit segment selectors.
- When writing user mode apps, programmers generally create segment selectors with assembler directives. If writing system code, programmers may need to create segment selectors directly. 
- When using the flat (unsegmented) memory model, segment registers are loaded with segment selectors that point to overlapping segments, each of which begins at address 0 of the linear address space.
- These overlapping segments then comprise the linear address space for the program. 
- Typically, two overlapping segments are defined: one for code and another for data and stacks. The CS segment register points to the code segment and all the other segment registers point to the data and stack segment.

 <p align="center"> 
    <img src="https://i.imgur.com/AbhbgWY.png" width="700px" height="auto">
</p

- When using the segmented memory model, each segment register is ordinarily loaded with a different segment selector so that each segment register points to a different segment within the linear address space.
- At any time, a program can thus access up to six segments in the linear address space. To access a segment not pointed to by one of the segment registers, a program must first load the segment selector for the segment to be accessed into a segment register.

 <p align="center"> 
    <img src="https://i.imgur.com/fu7EMkz.png" width="700px" height="auto">
</p

- Each of the segment registers is associated with one of three types of storage: code, data, or stack.
- The DS, ES, FS, and GS registers point to four data segments.
- The CS register contains the segment selector for the code access.
- The SS register contains the segment selector for the stack segment.
- If you dont precise the segment, the compiler will use explicitely:
    - CS for code access , DS for data access and SS for stack access.
    - The processor fetches instructions from the code segment, using a logical address that consists of the segment selector in the CS register and the contents of the EIP register.
    - The CS register cannot be loaded explicitly by an application program (Otherwise you get: EXCEPTION_ILLEGAL_INSTRUCTION).
    -  Stack operations such as push and pop, as well as memory references using the stack pointer %esp or base pointer register %ebp, use the stack segment %ss. 
    - Other memory references use the data segment %ds.
    - String operations additionally use the extra segment %es.
    - Windows make use of FS to store the Win32 Thread Information Block (TIB) data structure.
 
 <p align="center"> 
    <img src="https://i.imgur.com/cixb0xe.png" width="700px" height="auto">
</p
