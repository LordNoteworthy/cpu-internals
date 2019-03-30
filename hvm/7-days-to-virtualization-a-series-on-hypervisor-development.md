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
- Each virtual processor has their own set of items listed above and below.
  - A processor context (vCPU context; container for important structures for VMX operation)
  - A VMXON region
  - A VMCS region
  - A processor stack
- Pre-VMX Operation Requirements:
  - IA32_VMX_CR0_FIXED0
  - IA32_VMX_CR0_FIXED1
  - IA32_VMX_CR4_FIXED0
  - IA32_VMX_CR4_FIXED1
  - Enabling VMX Operation in CR4
  - Enable the use of VMXON in general operations (inside and outside SMX) - IA32_FEATURE_CONTROL
- Entering VMX Operation:
  - Check if VMX operation is supported by the processor.
  - Allocate system and processor specific structures.
  - Adjust CR4 and CR0 to supported values.
  - Enable VMX operation on each processor.
  - Set feature control bits to allows use of vmxon.
  - Execute vmxon, and enter VMX operation.
  - Remember when I mentioned you’ll want to check flags for success of certain instructions? This is one of those instructions. If it fails, the CF (carry flag) in RFLAGS is set. We’ll write a custom intrinsic to perform this check.
