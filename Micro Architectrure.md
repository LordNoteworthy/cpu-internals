
## Memory-mapped IO vs Port-mapped iO
* If the device is accessible in a dedicated address space, separate from the address space or memory, then the type of IO is called __port-mapped IO__ or __isolated IO (PMIO)__.
* If the device is accessible as part of an unique address space, where memory is also located, then the type of IO is called __memory-mapped IO (MMIO)__.

|Memory-mapped IO|Port-mapped IO|
|----------------|-------------|
|Same address bus to address memory and I/O devices|Different address spaces for memory and I/O devices|
|Access to the I/O devices using regular instructions|Uses a special class of CPU instructions to access I/O devices|
|Most widely used I/O method|x86 Intel microprocessors: IN and OUT instructions|
* http://www.bogotobogo.com/Embedded/memory_mapped_io_vs_port_mapped_isolated_io.php

## Programmed-IO vs Interrupt-Driven IO & Direct Memory Access
* In __Progammed I/O (PIO)__, the CPU manually check if there are any I/O requests available periodically __polling__. If there isn't, it keeps executing its normal workflow. If there is, it handles the IO request instead.
* In __Interrupt-Driven I/O__, the CPU doesn't need to manually check for IO requests. When there is an I/O request available, the CPU is immediately notified using __interrupts__, and the request is immediately handled using a interrupt service routines. The use of DMA allows interrupt-driven IO to be used. Otherwise, a system must use programmed I/O if DMA is not available.
* __Direct Memory Access (DMA)__ is a method allowing devices (typically has very slow I/O speeds) to access main memory without needing the CPU to explicitly handle the requests. When the CPU initiates data transfer from IO devices to main memory, the CPU instructs the DMA controller to handle this task. The CPU will then "forget" about this operation, and proceed with other tasks. When the DMA controller has completed the transfer, it will signal the CPU using an interrupt. The CPU will then "conclude" the task needed associated with the data transfer it initiated.

## Video RAM
__Video RAM (VRAM)__ is a dual-ported variant of dynamic RAM (DRAM), which was once commonly used to store the [framebuffer](https://en.wikipedia.org/wiki/Framebuffer) in graphics adapters. System RAM is great for loading and running programs, but when you need graphics power, VRAM is where it's at. This is the memory used to store image data that the computer displays; it acts as a buffer between the CPU and the video card. When a picture is to be displayed on the screen, the image is first read by the processor and then written to the VRAM. 
<p align="center"> 
    <img src="https://i.imgur.com/waiV3E0.png" width="600px" height="auto">
</p>

## PIC vs APIC
* __Programmable interrupt controller (PIC)__ one of the most important chips making up the x86 architecture, its role is to manage hardware interrupts and send them to the appropriate system interrupt. This allows the system to respond to devices needs without loss of time (from polling the device, for instance).
* The 8259 PIC controls the CPU's interrupt mechanism, by accepting several interrupt requests and feeding them to the processor in order.
* Each PIC has 8 input lines, called __Interrupt Requests (IRQ)__, numbered from 0 to 7.
* It is important to note that APIC has replaced the 8259 PIC in more modern systems, especially those with multiple cores/processors.
* The __Advanced Programmable Interrupt Controller (APIC)__ is used for sophisticated interrupt redirection, and for sending interrupts between processors. These things weren't possible using the older PIC specification.
* APIC enables the construction of multiprocessor systems, It is one of several architectural designs intended to solve interrupt routing efficiency issues in multiprocessor computer systems.
* The [APIC](https://wiki.osdev.org/APIC) is a split architecture design, with a local component (LAPIC) usually integrated into the processor itself, and an optional I/O APIC on a system bus.
* The `CPUID.01h:EDX[bit 9]` flag specifies whether a CPU has a built-in local APIC.

## IOMMU
* __Input–output memory management unit (IOMMU)__ is a memory management unit (MMU) that connects a DMA-capable I/O bus to the main memory.
* Like a traditional MMU, which translates CPU-visible virtual addresses to physical addresses, the IOMMU maps device-visible virtual addresses (also called device addresses or I/O addresses in this context) to physical addresses.
<p align="center"> 
    <img src="https://upload.wikimedia.org/wikipedia/commons/d/d6/MMU_and_IOMMU.svg" width="400px" height="auto">
</p>