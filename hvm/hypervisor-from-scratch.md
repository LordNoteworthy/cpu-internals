Some notes I wrote down while reading [Hypervisor from scratch tutorials](https://rayanfam.com/topics/hypervisor-from-scratch-part-1/)

### Part 1

- Personally, as Linux kernel manages faults like #GP and other exceptions and tries to avoid kernel panic and keep the system up so it’s better for testing something like hypervisor or any CPU-related affair. On the other hand, Windows never tries to manage any unexpected exception and it leads to a blue screen of death whenever you didn’t manage your exceptions, thus you might get lots of BSODs. Btw, you’d better test it on both platforms (and other platforms too.).

### Part 2

- For optimizing VMM, you can use Fast I/O which is a different way to initiate I/O operations that are faster than IRP. Fast I/O operations are always synchronous.
- Fast I/O is specifically designed for rapid synchronous I/O on cached files. In fast I/O operations, data is transferred directly between user buffers and the system cache, bypassing the file system and the storage driver stack. (Storage drivers do not use fast I/O.) If all of the data to be read from a file is resident in the system cache when a fast I/O read or write request is received, the request is satisfied immediately.
- When the I/O Manager receives a request for synchronous file I/O (other than paging I/O), it invokes the fast I/O routine first. If the fast I/O routine returns TRUE, the operation was serviced by the fast I/O routine. If the fast I/O routine returns FALSE, the I/O Manager creates and sends an IRP instead.
- Detecting Hypervisor Support:
  - You could know the presence of VMX using CPUID if `CPUID.1:ECX.VMX[bit 5] = 1`, then VMX operation is supported.
  - First of all, we need to know if we’re running on an Intel-based processor or not, this can be understood by checking the CPUID instruction and find vendor string “GenuineIntel“. (EAX = 0).
- Enabling VMX Operation:
  - Before system software can enter VMX operation, it enables VMX by setting `CR4.VMXE[bit 13] = 1`.

### Part 3

- For VMM dev, it is important to retrieve the count of logical processors using KeQueryActiveProcessors(), then we should pass a KAFFINITY mask to the KeSetSystemAffinityThread() which sets the system affinity of the current thread. So we can enable VMX on all LP.
- VMXON Regions and VMCS Regions use physical address as the operand to VMXON and VMPTRLD instruction so we should create functions to convert Virtual Address to Physical address: `MmGetPhysicalAddress(va).QuadPart;`
- And as long as we can’t directly use physical addresses for our modifications in protected-mode then we have to convert physical address to virtual address. `MmGetVirtualForPhysical(PhysicalAddr);`
- You should maintain the VMCS region and related structures in writeback cacheable memory.

### Part 4

- Shadow Page Table is not recommended today as always lead to VMM traps (which result in a vast amount of VM-Exits) and losses the performance due to the TLB flush on every switch and another caveat is that there is a memory overhead due to shadow copying of guest page tables.
- In EPT:
  - One page table is maintained by guest OS, which is used to generate the guest-physical address.
  - The other page table is maintained by VMM, which is used to map guest physical address to host physical address.
- Extended Page Table vs Shadow Page Table:
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
      - Permission emulation
- EPT is used when the “enable EPT” VM-execution control is 1. It translates the guest-physical addresses used in VMX non-root operation and those used by VM entry for event injection: <p align="center"><img src="https://i.imgur.com/uI1VUrO.png"  width="500px" height="auto"></p>.
- By the way, N is the physical-address width supported by the processor. CPUID with **80000008H** in EAX gives you the supported width in EAX bits 7:0.

### Part 5

```
typedef struct _VirtualMachineState
{
    UINT64 VMXON_REGION;                    // VMXON region
    UINT64 VMCS_REGION;                     // VMCS region
    UINT64 EPTP;                            // Extended-Page-Table Pointer
    UINT64 VMM_Stack;                       // Stack for VMM in VM-Exit State
    UINT64 MSRBitMap;                       // MSRBitMap Virtual Address
    UINT64 MSRBitMapPhysical;               // MSRBitMap Physical Address
} VirtualMachineState, *PVirtualMachineState;
```

- If a VM-Exit occurs, the guest RIP remains constant and it’s up to you to change the Guest RIP or not so if you don’t have a special function for managing this situation then you execute a VMRESUME and it’s like an infinite loop of executing CPUID and VMRESUME because you didn’t change the RIP.
- In order to solve this problem you have to read a VMCS field called VM_EXIT_INSTRUCTION_LEN that stores the length of the instruction that caused the VM-Exit so you have to first, read the GUEST current RIP, second the VM_EXIT_INSTRUCTION_LEN and third add it to GUEST RIP. Now your GUEST RIP points to the next instruction and you’re good to go.

### Part 6

- For the CPU_BASED_VM_EXEC_CONTROL, we set CPU_BASED_ACTIVATE_MSR_BITMAP, this way you can enable MSR BITMAPs filter (described later in this part). Setting this field is somehow mandatory as you might guess, Windows access lots of MSRs during a simple kernel execution so if you don’t set this bit, then you’ll exit on each MSR access and of course, your VMX Exit-Handler is called so clearing this bit to zero makes the system notably slower.
- For the SECONDARY_VM_EXEC_CONTROL, we use CPU_BASED_CTL2_RDTSCP to enable RDTSCP, CPU_BASED_CTL2_ENABLE_INVPCID to enable INVPCID and the CPU_BASED_CTL2_ENABLE_XSAVE_XRSTORS to enable XSAVE and XRSTORS.
- It’s because I run the above code in my Windows 10 1809 and see Windows uses INVPCID and XSAVE for it’s internal use (in the processors that support these features), so if you didn’t enable them before virtualizing the core, then it probably lead to error.