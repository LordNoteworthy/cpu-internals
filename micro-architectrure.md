### Bus

- The idea of a bus is simple, it lets you connect components to the computer's processor. 
- The graphics card talks to the processor using the computer's bus as a communication path.
- The advantage of a bus is that it makes parts more interchangeable

### System bus or local bus

- is a single bus that connects the major components of a computer system, combining the functions of a data bus to carry information, an address bus to determine where it should be sent, and a control bus to determine its operation.
- The technique was developed to reduce costs and improve modularity, and although popular in the 1970s and 1980s, more modern computers use a variety of separate buses adapted to more specific needs.
- Is the fastest bus in the system.

<p align="center"> 
    <img src="https://i.imgur.com/XoCAfrl.png" width="400px" height="auto">
</p>

### Slower Bus

- for communicating with things like hard disks and sound cards.
- One very common bus of this type is known as the PCI bus.
- These slower buses connect to the system bus through a bridge, which is a part of the computer's chipset and acts as a traffic cop, integrating the data from the other buses to the system bus.

### Peripheral Component Interconnect

- PCI presents a hybrid of sorts between ISA and VL-Bus.
- It provides direct access to system memory for connected devices, but uses a bridge to connect to the frontside bus and therefore to the CPU.
- Basically, this means that it is capable of even higher performance than VL-Bus while eliminating the potential for interference with the CPU.

<p align="center"> 
    <img src="https://i.imgur.com/EpWARck.png" width="300px" height="auto">
</p>

### Serial Bus

- Many communication systems were generally designed to connect two integrated circuits on the same printed circuit board, connected by signal traces on that board (rather than external cables).
- Integrated circuits are more expensive when they have more pins. To reduce the number of pins in a package, many ICs use a serial bus to transfer data when speed is not important. Some examples of such low-cost serial buses include RS-232, SPI, I²C, DC-BUS, UNI/O, 1-Wire and PCI Express.

### Memory-mapped IO vs Port-mapped IO

- If the device is accessible in a dedicated address space, separate from the address space or memory, then the type of IO is called **port-mapped IO** or **isolated IO (PMIO)**.
- If the device is accessible as part of an unique address space, where memory is also located, then the type of IO is called **memory-mapped IO (MMIO)**.

| Memory-mapped IO                                     | Port-mapped IO                                                 |
| ---------------------------------------------------- | -------------------------------------------------------------- |
| Same address bus to address memory and I/O devices   | Different address spaces for memory and I/O devices            |
| Access to the I/O devices using regular instructions | Uses a special class of CPU instructions to access I/O devices |
| Most widely used I/O method                          | x86 Intel microprocessors: IN and OUT instructions             |

- http://www.bogotobogo.com/Embedded/memory_mapped_io_vs_port_mapped_isolated_io.php

### Programmed-IO vs Interrupt-Driven IO & Direct Memory Access

- In **Progammed I/O (PIO)**, the CPU manually check if there are any I/O requests available periodically **polling**. If there isn't, it keeps executing its normal workflow. If there is, it handles the IO request instead.
- In **Interrupt-Driven I/O**, the CPU doesn't need to manually check for IO requests. When there is an I/O request available, the CPU is immediately notified using **interrupts**, and the request is immediately handled using a interrupt service routines. The use of DMA allows interrupt-driven IO to be used. Otherwise, a system must use programmed I/O if DMA is not available.
- **Direct Memory Access (DMA)** is a method allowing devices (typically has very slow I/O speeds) to access main memory without needing the CPU to explicitly handle the requests. When the CPU initiates data transfer from IO devices to main memory, the CPU instructs the DMA controller to handle this task. The CPU will then "forget" about this operation, and proceed with other tasks. When the DMA controller has completed the transfer, it will signal the CPU using an interrupt. The CPU will then "conclude" the task needed associated with the data transfer it initiated.

### Video RAM

**Video RAM (VRAM)** is a dual-ported variant of dynamic RAM (DRAM), which was once commonly used to store the [framebuffer](https://en.wikipedia.org/wiki/Framebuffer) in graphics adapters. System RAM is great for loading and running programs, but when you need graphics power, VRAM is where it's at. This is the memory used to store image data that the computer displays; it acts as a buffer between the CPU and the video card. When a picture is to be displayed on the screen, the image is first read by the processor and then written to the VRAM.

<p align="center"> 
    <img src="https://i.imgur.com/waiV3E0.png" width="600px" height="auto">
</p>

### PIC vs APIC

- **Programmable interrupt controller (PIC)** one of the most important chips making up the x86 architecture, its role is to manage hardware interrupts and send them to the appropriate system interrupt. This allows the system to respond to devices needs without loss of time (from polling the device, for instance).
- The 8259 PIC controls the CPU's interrupt mechanism, by accepting several interrupt requests and feeding them to the processor in order.
- Each PIC has 8 input lines, called **Interrupt Requests (IRQ)**, numbered from 0 to 7.
- It is important to note that APIC has replaced the 8259 PIC in more modern systems, especially those with multiple cores/processors.
- The **Advanced Programmable Interrupt Controller (APIC)** is used for sophisticated interrupt redirection, and for sending interrupts between processors. These things weren't possible using the older PIC specification.
- APIC enables the construction of multiprocessor systems, It is one of several architectural designs intended to solve interrupt routing efficiency issues in multiprocessor computer systems.
- The [APIC](https://wiki.osdev.org/APIC) is a split architecture design, with a local component (LAPIC) usually integrated into the processor itself, and an optional I/O APIC on a system bus.
- The `CPUID.01h:EDX[bit 9]` flag specifies whether a CPU has a built-in local APIC.

### PIT

- **Programmable Interval Timer (PIT)** is a counter that generates an output signal when it reaches a programmed count. 
- The output signal may trigger an interrupt.
- PITs may be one-shot or periodic.
- One-shot timers will signal only once and then stop counting.
- Periodic timers signal every time they reach a specific value and then restart, thus producing a signal at periodic intervals.

### Input–output memory management unit (IOMMU)

- is a memory management unit (MMU) that connects a DMA-capable I/O bus to the main memory.
- Like a traditional MMU, which translates CPU-visible virtual addresses to physical addresses, the IOMMU maps device-visible virtual addresses (also called device addresses or I/O addresses in this context) to physical addresses.
- IOMMU allows guest VMs to directly use peripheral devices, such as Ethernet, accelerated graphics cards, and hard-drive controllers, through DMA and **interrupt remapping**. This is sometimes called **PCI passthrough**.
  <p align="center"> <img src="https://upload.wikimedia.org/wikipedia/commons/d/d6/MMU_and_IOMMU.svg" width="400px" height="auto">
  </p>

### Non-Maskable Interrupt (NMI)

- Is a hardware interrupt that standard interrupt-masking techniques in the system cannot ignore.
- It typically occurs to signal attention for non-recoverable hardware errors. (Some NMIs may be masked, but only by using proprietary methods specific to the particular NMI.)

### Inter-processor interrupts (IPIs)

- Is a special type of interrupt by which one processor may interrupt another processor in a multiprocessor system if the interrupting processor requires action from the other processor.
- Actions that might be requested include:
  - flushes of memory management unit caches, such as translation lookaside buffers, on other processors when memory mappings are changed by one processor
  - stopping when the system is being shut down by one processor.

### E820

- e820 is shorthand to refer to the facility by which the BIOS of x86-based computer systems reports the memory map to the operating system or boot loader.
- It is accessed via the int 15h call, by setting the AX register to value E820 in hexadecimal. It reports which memory address ranges are usable and which are reserved for use by the BIOS.

### Bus snooping

- Bus snooping or bus sniffing is a scheme by which a coherency controller (snooper) in a cache monitors or snoops the bus transactions, and its goal is to maintain a cache coherency in distributed shared memory systems.
- A cache containing a coherency controller (snooper) is called a snoopy cache.

### Load-Store Architcture

- is an ISA that divides instructions into two categories:
    - memory access (load and store between memory and registers)
    - and ALU operations (which only occur between registers).
- RISC ISAs such as PowerPC, SPARC, RISC-V, ARM, and MIPS are load–store architectures.
- For instance, in a load–store approach both operands and destination for an ADD operation must be in registers. This differs from a register–memory architecture (for example, a CISC instruction set architecture such as x86) in which one of the operands for the ADD operation may be in memory, while the other is in a register