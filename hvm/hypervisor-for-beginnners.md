These are somes notes I made while reading [Hypervisor-for-beginners]https://github.com/rohaaan/hypervisor-for-beginners) source code.

1. At the start of DriverEntry, call GetE820FromRegistry to retrieve total physical memory
   - Read ".Translated" in "\\Registry\\machine\\HARDWARE\\RESOURCEMAP\\System Resources\\Physical Memory".
2. InitEptTable
   - g_EptNumberOfPages = CountForPages(g_TotalPhysicalMemory);
   - g_ept.pml4 = MmAllocateNonCachedMemory(PAGE_SIZE);
   - Loop over nuumber of pages: g_ept.pages[i] = MmAllocateNonCachedMemory(PAGE_SIZE);
   - Ept11mapping:
     - For each memory range, depending on its type: EptMapRage(Start, Length, CACHE_TYPE_WB);
3. Enter VMM
   - Loop over LP and call InitializeVMMInitState
     - Allocate in non cache memory with MmAllocateNonCachedMemory (PAGE_SIZE) VMXONRegion, VMCSRegion, MsrBitmap.
     - Allocate in non paged pool: VMMStack (VMM_STACK_SIZE)
   - RunOnProcessor:
     - KeRaiseIrqlToDpcLevel
     - StartVMX(ASM) save the state and call doStartVMX.
     - EnableVMX(pCurrentVMMInitState);
       - If CPUID.1:ECX.VMX[bit 5] = 1, then VMX operation is supported.
       - Determine the VMX capabilities
         - save the revision_id
         - Determine the default1 and default0 for control msrs
         - detect EPT and VPID capability
       - terminate if ept or write back memory cache are not suported.
       - set CR4 (X86_CR4_VMXE) to enable VMX.
       - Write the revision id.
       - VmxTurnOn (vmxon)
     - VmxSetupVMCS
       - \*(PULONG)VMMInitState->VMCSRegion = IA32BasicMsr.RevId;
       - VmxClear(VMMInitState->PhysicalVMCSRegion);
       - VmxPtrld(VMMInitState->PhysicalVMCSRegion);
       - VmxWrite(GUEST_RSP, (ULONG64) GuestRsp); //setup guest sp
       - VmxWrite(GUEST_RIP, (ULONG64) GuestRip); //setup guest ip
       - Set MSR bitmap, need all 0 to stop vmexit
       - VmxWrite(HOST_RSP, ((ULONG64) VMMInitState->VMMStack + VMM_STACK_SIZE - 1));
       - VmxWrite(HOST_RIP, (ULONG64) VmxExitHandler);
     - EptInit
       - PhysicalAddress.QuadPart = g_ept.pml4_phys;
       - VmxWrite(EPT_POINTER, PhysicalAddress.LowPart | EPT_PAGEWALK_LENGTH_4 | EPT_POINTER_EPT_WB);
       - VmxWrite(EPT_POINTER_HIGH, PhysicalAddress.HighPart);
     - VmxLaunch
4. VmxExitHandler (ASM): HVM_SAVE_ALL_NOSEGREGS => call doVmxExitHandler => HVM_RESTORE_ALL_NOSEGREGS => vmx_resume.
5. doVmxExitHandler
   - ulExitReason = (ULONG)VmxRead(VM_EXIT_REASON);
   - case EXIT_REASON_VMLAUNCH:
   - case EXIT_REASON_EXCEPTION_NMI:
   - case EXIT_REASON_CPUID:
   - case EXIT_REASON_INVD:
   - case EXIT_REASON_VMCALL:
   - case EXIT_REASON_CR_ACCESS:
   - case EXIT_REASON_MSR_READ:
   - case EXIT_REASON_MSR_WRITE:
   - case EXIT_REASON_EPT_VIOLATION:
