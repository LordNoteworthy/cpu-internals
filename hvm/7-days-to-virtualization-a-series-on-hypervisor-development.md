Some notes I wrote down while reading [7 Days to Virtualization: A Series on Hypervisor Development](https://revers.engineering/7-days-to-virtualization-a-series-on-hypervisor-development/)

### Day 0: Virtual Environment Setup, Scripts, and WinDbg

- Disable Windows Defender
- Disable UAC (User Account Control)
- Disable Hyper-V, and Virtualization Based Security
- Configure DebugView:
  - Navigate to the registry tree HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager.
  - Create a new Key called _Debug Print Filter_.
  - Select the new key and create a new DWORD value named _IHVDRIVER_ with the value 0xFFFF.
  - Enable Capture Kernal and Verbose Debug Output.

### Day 1: Introduction to Virtualization, Type Definitions, and Support Testing

- Type 1 vs Type 2 hypervisors
- Para vs Full virtualization
- VMX operations
- Type definitions:
  - **IA32_EFER_MSR (0xC0000080)**: determining processor operation mode and whether the execute-disable bits for paging structures are available.
  - **IA32_FEATURE_CONTROL_MSR (0x3A)**: This is used prior to entering VMX operation, and without the bit fields the function enabling VMX operation would be littered with preprocessor defines. The three fields of interest are lock, vmxon_inside_smx, and vmxon_outside_smx.
  - **IA32_VMX_MISC_MSR (0x485)**
  - **IA32_VMX_BASIC_MSR (0x480)**: This MSR is used when we begin initializing our VMXON and VMCS region
  - **IA32_VMX_PINBASED_CTL_MSR (0x481)**
  - **IA32_VMX_PRIMARY_PROCESSOR_BASED_CTL_MSR (0x482)**
  - **IA32_VMX_SECONDARY_PROCESSOR_BASED_CTL_MSR (0x48B)**
  - **IA32_VMX_EXIT_CTL_MSR (0x483)**
  - **IA32_VMX_ENTRY_CTL_MSR (0x484)**
    All of the above definitions starting from IA32_VMX_PINBASED_CTL_MSR are used for controlling which conditions control trapping into the VMM, control certain processor components such as the ability to perform TSC scaling and offsetting, and operations to perform upon entering root or non-root operation.
  - **CR0**: primarily used to disable caching or paging, as well as enable protected mode for a processor.
  - **CR4**: used most notably when we intend to enter VMX operation we must set the vmx_enable bit and write the new control value back to CR4 for vmxon to work properly.
  - **CR3**: isn’t going to be used in our basic hypervisor, however, if you decide to implement your own memory management facilities having a CR3 definition will simplify a lot of bit manipulation that would be required otherwise.
  - **DR Registers**: definitions are important when dealing with exception bitmaps, particularly if you want to build a hypervisor level debugger.
    CPUID Definition
  - **CPUID**: used to obtain information about supported features that will be reported back through the feature_ecx union and allow the author to check whether the virtual_machine_extensions bit is set indicating support on the CPU.
  - **RFLAGS** is incredibly useful since some VMX instructions will set flags in the RFLAGS register if an error is encountered.

### Day 2: Entering VMX Operation, Explaining Implementation Requirements

- **Processor**: refers to the physical hardware. The component responsible for all processing operations. A machine can have more than one processor, just as some servers have a bi-processor setup. This is also referred to as a **socket**, **CPU**, or **package**.
- **Core**: within a physical processor there is an operation unit called a core. It’s often said that a core is like a processor, so a single processor with 2 cores means it has a single physical piece of hardware with two independent processing units that can fetch and execute instructions.
- **logical processor** is the number of threads a machine can handle at the same time. This is an abstraction of a processor, at least on Windows. It’s able to fetch and execute its own stream of instructions in the same processor time slot:
  `count of LP = cores per processor * the thread count.`
- In virtualization literature you may see the term **virtual processor (VP)** used, however, by today’s standards a virtual processor is equivalent to a **physical core**. That means if you need to allocate a number of virtual processors for your VM you determine the number of cores in your physical processor and assign however many that is.
- VMM context is our hypervisor control structure which will contain:
  - a pointer to the VMM stack (or host stack)
  - a table of vCPU contexts
  - and a processor count.
- Unique objects to any given virtual CPU:
  - It’s own set of control registers.
  - It’s own set of debug registers.
  - It’s own set of model specific registers.
  - It’s own set of special registers (GDTR, LDTR, IDTR)
  - It’s own status register (RFLAGS)
  - It’s own general purpose registers (RAX-R15)
  - It’s own segment information
- Each virtual processor has their own set of items (required for VMX operation):
  - A processor context (vCPU context; container for important structures for VMX operation)
  - A VMXON region
  - A VMCS region
  - A processor stack
- Pre-VMX Operation Requirements:
  - IA32_VMX_CR0_FIXED0
  - IA32_VMX_CR0_FIXED1
  - IA32_VMX_CR4_FIXED0
  - IA32_VMX_CR4_FIXED1
  - Enabling VMX Operation in CR4 (`cr4.vmx_enable`)
  - Set `feature_msr.vmxon_outside_smx` and `feature_msr.lock` bits in `IA32_FEATURE_CONTROL` msr if `feature_msr.bits.lock==0`.
- Entering VMX Operation:
  - Check if VMX operation is supported by the processor.
  - Allocate system and processor specific structures.
    - VMM context in the non-paged pool, as well as our vCPU table and stack.
  - Adjust CR4 and CR0 to supported values.
  - Enable VMX operation on each processor.
  - Set feature control bits to allows use of vmxon.
  - Execute vmxon, and enter VMX operation.
  - Remember when I mentioned you’ll want to check flags for success of certain instructions? This is one of those instructions. If it fails, the CF (carry flag) in RFLAGS is set. We’ll write a custom intrinsic to perform this check.

### Day 3: The VMCS, Component Encoding, and Multiprocessor Initialization

- The VMX instructions abstract access to the virtualization state so that implementation specific data isn’t at risk of being incorrectly modified.
- It’s also another reason Intel performs component encoding (explained in the next subsection), that way all indexes into the VMCS start at a specific offset and can be consistent across implementations.
- **vmxread|vmxwrite**: If executed in non-root operation it will read/write from the VMCS referenced in the VMCS link pointer field in the current VMCS. If the link pointer is invalid, it will trap into the hypervisor and execute the proper exit handler for this instruction.
- This macro properly generates the 32-bit encoding value for VMCS components, and now we can begin encoding our individual VMCS components. You could go to an existing project like KVM and copy out the definitions with the proper encoding value already defined, however, I believe that’s a poor idea given that on any given processor the values are subject to change. We want to be able to find and generate these encodings with ease and flexibility should new fields be introduced.
- The fourth control field in the table is the Guest/Host Mask and Read Shadows for CR0, and CR4. These fields control execution of instructions that access those registers. In general, special VMCS control-components allow your VMM to modify values read from CR0 and CR4. If the bits in the guest/host mask are set to 1 then they are owned by the host which means if the guest attempts to set them to values different from the bits in the read shadow for the respective control register then a VM exit will occur. Any guest that reads the values for these bits through use of typical instructions will read values from the read shadow for the respective control register. If bits are cleared to 0 in the guest/host mask then they are owned by the guest, meaning that any attempt by the guest to modify or read them succeeds and returns the bits from the respective control register.
- IPI:
  - The ability to use inter-processor interrupts is available because of support from the programming interrupt controllers (Intel’s APIC/x2APIC.) An example of their use is at boot, all interrupts are delivered to an arbitrarily selected processor core – this core is then referred to as the **bootstrap processor (BSP)**. The selection of the bootstrap processor is done by system hardware, and all other processors / cores are designated as application processors (AP).
  - It’s important to understand this facility provided by the APIC/x2APIC. However, we won’t be using it for our method of initialization. This is the primary method of initialization for type-1 hypervisors, and can be used by type-2 hypervisors. The best solution on a type-2 hypervisor is to use the operating system facilities provided to initialize the VMM binary on all processors sequentially. Using an IPI either through the use of KeIpiGenericCall, or writing our own subroutine, limits what we can do inside of the IPI callback (the function that executes when the IPI is received) because the IRQL of the callback is raised to IPI_LEVEL (29). Very few operating system facilities can be used at this level, and we’re trying to make this project as simple as possible.
- Affinity Masks:
  - Processor affinity refers to the binding of a thread to a specific processing unit so that the thread will run on the designated processor. 
- Deferred Procedure Calls
  - Most commonly known for the usage in ISR servicing.
  - To queue a DPC for the current processor we would call KeInsertQueueDpc following the construction of our own DPC object. 
  - It is possible to select a processor that a DPC object will be executed on using KeSetTargetProcessorDpc.
- We need to do the following per processor (as per the specification as well):
  - Use cpuid on each virtual processor to determine if VMX is supported.
  - Check VMCS and VMXON revision identifiers for each virtual processor.
  - Check the VMX capability MSR’s of each virtual processor for value restrictions (allowed 0, or 1 bits).
  - Allocate and initialize VMXON and VMCS regions on each virtual processor.
  - Adjust CR0 and CR4 to support all fixed bits reported in the fixed MSR’s.
  - Enable VMX operation on each virtual processor.
  - Validate that the IA32_FEATURE_CONTROL MSR has been properly programmed and the lock bit set.
  - Execute VMXON for each virtual processor.
  - Error handling.
- We don’t want to exit on any MSR accesses for this project, so we need to allocate an MSR bitmap (recall it is 4-KByte in size) and zero it out so that all bits in the bitmap are 0.
- Lucky for us, the Microsoft specific __vmx_on intrinsic used in our project does all the error checking for us. The particular check of interest is to see if RFLAGS.CF was 0, if is was set that would indicate that vmxon failed and without the Microsoft specific intrinsic we’d be left doing all the checks ourselves