
## Memory-mapped IO vs Port-mapped iO
* If the device is accessible in a dedicated address space, separate from the address space or memory, then the type of IO is called *port-mapped IO* or *isolated IO* (PMIO)
* If the device is accessible as part of an unique address space, where memory is also located, then the type of IO is called *memory-mapped IO* (MMIO).

|Memory-mapped IO|Port-mapped IO|
|----------------|-------------|
|Same address bus to address memory and I/O devices|Different address spaces for memory and I/O devices|
|Access to the I/O devices using regular instructions|Uses a special class of CPU instructions to access I/O devices|
|Most widely used I/O method|x86 Intel microprocessors: IN and OUT instructions|
* http://www.bogotobogo.com/Embedded/memory_mapped_io_vs_port_mapped_isolated_io.php

## Programmed-IO vs Interrupt-Driven IO & Direct Memory Access
* In *Progammed I/O* (PIO), the CPU manually check if there are any I/O requests available periodically `polling`. If there isn't, it keeps executing its normal workflow. If there is, it handles the IO request instead.
* in *Interrupt-Driven I/O*, the CPU doesn't need to manually check for IO requests. When there is an I/O request available, the CPU is immediately notified using `interrupts`, and the request is immediately handled using a interrupt service routines. The use of DMA allows interrupt-driven IO to be used. Otherwise, a system must use programmed I/O if DMA is not available.
* *Direct Memory Access* (DMA) is a method allowing devices (typically has very slow I/O speeds) to access main memory without needing the CPU to explicitly handle the requests. When the CPU initiates data transfer from IO devices to main memory, the CPU instructs the DMA controller to handle this task. The CPU will then "forget" about this operation, and proceed with other tasks. When the DMA controller has completed the transfer, it will signal the CPU using an interrupt. The CPU will then "conclude" the task needed associated with the data transfer it initiated.
