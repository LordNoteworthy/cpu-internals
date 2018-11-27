Some notes I wrote down while reading [Hypervisor from scratch tutorials](https://rayanfam.com/topics/hypervisor-from-scratch-part-1/)

### Part I
* Personally, as Linux kernel manages faults like #GP and other exceptions and tries to avoid kernel panic and keep the system up so it’s better for testing something like hypervisor or any CPU-related affair. On the other hand, Windows never tries to manage any unexpected exception and it leads to a blue screen of death whenever you didn’t manage your exceptions, thus you might get lots of BSODs.By the way, you’d better test it on both platforms (and other platforms too.).

### Part II
* For optimizing VMM, you can use Fast I/O which is a different way to initiate I/O operations that are faster than IRP. Fast I/O operations are always synchronous.
* Fast I/O is specifically designed for rapid synchronous I/O on cached files. In fast I/O operations, data is transferred directly between user buffers and the system cache, bypassing the file system and the storage driver stack. (Storage drivers do not use fast I/O.) If all of the data to be read from a file is resident in the system cache when a fast I/O read or write request is received, the request is satisfied immediately. 
* When the I/O Manager receives a request for synchronous file I/O (other than paging I/O), it invokes the fast I/O routine first. If the fast I/O routine returns TRUE, the operation was serviced by the fast I/O routine. If the fast I/O routine returns FALSE, the I/O Manager creates and sends an IRP instead.
* Detecting Hypervisor Support:
    - You could know the presence of VMX using CPUID if CPUID.1:ECX.VMX[bit 5] = 1, then VMX operation is supported.
    - First of all, we need to know if we’re running on an Intel-based processor or not, this can be understood by checking the CPUID instruction and find vendor string “GenuineIntel“. (EAX = 0).
* Enabling VMX Operation:
    - Before system software can enter VMX operation, it enables VMX by setting ```CR4.VMXE[bit 13] = 1```.

### Part III
* For VMM dev, it is important to retrieve the count of logical processors using KeQueryActiveProcessors(), then we should pass a KAFFINITY mask to the KeSetSystemAffinityThread() which sets the system affinity of the current thread. So we can enable VMX on all LP.
* VMXON Regions and VMCS Regions use physical address as the operand to VMXON and VMPTRLD instruction so we should create functions to convert Virtual Address to Physical address: ```MmGetPhysicalAddress(va).QuadPart;```
* And as long as we can’t directly use physical addresses for our modifications in protected-mode then we have to convert physical address to virtual address. ```MmGetVirtualForPhysical(PhysicalAddr);```
* You should maintain the VMCS region and related structures in writeback cacheable memory.

### Part IIII
* Shadow Page Table is not recommended today as always lead to VMM traps (which result in a vast amount of VM-Exits) and losses the performance due to the TLB flush on every switch and another caveat is that there is a memory overhead due to shadow copying of guest page tables.
* In EPT:
    - One page table is maintained by guest OS, which is used to generate the guest-physical address.
    - The other page table is maintained by VMM, which is used to map guest physical address to host physical address.
* Extended Page Table vs Shadow Page Table:
    - EPT:
        - Walk any requested address
            - Appropriate to programs that have a large amount of page table miss when executing
            - Less chance to exit VM (less context switch)
        - Two-layer EPT
            - Means each access needs to walk two tables
        - Easier to develop
            - Many particular registers
            - Hardware helps guest OS to notify the VMM
    - SPT:
        - Only walk when SPT entry miss
            - Appropriate to programs that would access only some addresses frequently
            - Every access might be intercepted by VMM (many traps)
        - One reference
            - Fast and convenient when page hit
        - Hard to develop
            - Two-layer structure
            - Complicated reverse map
            -Permission emulation
* EPT is used when the “enable EPT” VM-execution control is 1. It translates the guest-physical addresses used in VMX non-root operation and those used by VM entry for event injection: <p align="center"><img src="https://i.imgur.com/uI1VUrO.png"  width="500px" height="auto"></p>.
* In addition to translating a guest-physical address to a host physical address, EPT specifies the privileges that software is allowed when accessing the address. Attempts at disallowed accesses are called __EPT violations__ and cause VM-exits.
* Keep in mind that address never translates through EPT, when there is no access. That your guest-physical address is never used until there is access (Read or Write) to that location in memory.
* There are other types of implementing page walks (2 or 3 level paging) and if you set the 7th bit of PDPTE (Maps 1 GB) or the 7th bit of PDE (Maps 2 MB) so instead of implementing 4 level paging (like what we want to do for the rest of the topic) you set those bits but keep in mind that the corresponding tables are different. These tables described in (Table 28-4. Format of an EPT Page-Directory Entry (PDE) that Maps a 2-MByte Page) and (Table 28-2. Format of an EPT Page-Directory-Pointer-Table Entry (PDPTE) that Maps a 1-GByte Page). Alex Ionescu’s SimpleVisor is an example of implementing in this way.
* By the way, N is the physical-address width supported by the processor. CPUID with __80000008H__ in EAX gives you the supported width in EAX bits 7:0.