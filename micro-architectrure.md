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
