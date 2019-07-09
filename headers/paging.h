typedef struct _PML4E
{
    union {
        struct
        {
            ULONG64 Present : 1;          // Must be 1, region invalid if 0.
            ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
            ULONG64 UserSupervisor : 1;   // If 0, user-mode accesses not allowed.
            ULONG64 PageWriteThrough : 1; // Determines the memory type used to access PDPT.
            ULONG64 PageCacheDisable : 1; // Determines the memory type used to access PDPT.
            ULONG64 Accessed : 1;         // If 0, this entry has not been used for translation.
            ULONG64 Ignored1 : 1;
            ULONG64 PageSize : 1; // Must be 0 for PML4E.
            ULONG64 Ignored2 : 4;
            ULONG64 PageFrameNumber : 36; // The page frame number of the PDPT of this PML4E.
            ULONG64 Reserved : 4;
            ULONG64 Ignored3 : 11;
            ULONG64 ExecuteDisable : 1; // If 1, instruction fetches not allowed.
        };
        ULONG64 Value;
    };
} PML4E, *PPML4E;
static_assert(sizeof(PML4E) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _PDPTE
{
    union {
        struct
        {
            ULONG64 Present : 1;          // Must be 1, region invalid if 0.
            ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
            ULONG64 UserSupervisor : 1;   // If 0, user-mode accesses not allowed.
            ULONG64 PageWriteThrough : 1; // Determines the memory type used to access PD.
            ULONG64 PageCacheDisable : 1; // Determines the memory type used to access PD.
            ULONG64 Accessed : 1;         // If 0, this entry has not been used for translation.
            ULONG64 Ignored1 : 1;
            ULONG64 PageSize : 1; // If 1, this entry maps a 1GB page.
            ULONG64 Ignored2 : 4;
            ULONG64 PageFrameNumber : 36; // The page frame number of the PD of this PDPTE.
            ULONG64 Reserved : 4;
            ULONG64 Ignored3 : 11;
            ULONG64 ExecuteDisable : 1; // If 1, instruction fetches not allowed.
        };
        ULONG64 Value;
    };
} PDPTE, *PPDPTE;
static_assert(sizeof(PDPTE) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _PDE
{
    union {
        struct
        {
            ULONG64 Present : 1;          // Must be 1, region invalid if 0.
            ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
            ULONG64 UserSupervisor : 1;   // If 0, user-mode accesses not allowed.
            ULONG64 PageWriteThrough : 1; // Determines the memory type used to access PT.
            ULONG64 PageCacheDisable : 1; // Determines the memory type used to access PT.
            ULONG64 Accessed : 1;         // If 0, this entry has not been used for translation.
            ULONG64 Ignored1 : 1;
            ULONG64 PageSize : 1; // If 1, this entry maps a 2MB page.
            ULONG64 Ignored2 : 4;
            ULONG64 PageFrameNumber : 36; // The page frame number of the PT of this PDE.
            ULONG64 Reserved : 4;
            ULONG64 Ignored3 : 11;
            ULONG64 ExecuteDisable : 1; // If 1, instruction fetches not allowed.
        };
        ULONG64 Value;
    };
} PDE, *PPDE;
static_assert(sizeof(PDE) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

typedef struct _PTE
{
    union {
        struct
        {
            ULONG64 Present : 1;          // Must be 1, region invalid if 0.
            ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
            ULONG64 UserSupervisor : 1;   // If 0, user-mode accesses not allowed.
            ULONG64 PageWriteThrough : 1; // Determines the memory type used to access the memory.
            ULONG64 PageCacheDisable : 1; // Determines the memory type used to access the memory.
            ULONG64 Accessed : 1;         // If 0, this entry has not been used for translation.
            ULONG64 Dirty : 1;            // If 0, the memory backing this page has not been written to.
            ULONG64 PageAccessType : 1;   // Determines the memory type used to access the memory.
            ULONG64 Global : 1;           // If 1 and the PGE bit of CR4 is set, translations are global.
            ULONG64 Ignored2 : 3;
            ULONG64 PageFrameNumber : 36; // The page frame number of the backing physical page.
            ULONG64 Reserved : 4;
            ULONG64 Ignored3 : 7;
            ULONG64 ProtectionKey : 4;  // If the PKE bit of CR4 is set, determines the protection key.
            ULONG64 ExecuteDisable : 1; // If 1, instruction fetches not allowed.
        };
        ULONG64 Value;
    };
} PTE, *PPTE;
static_assert(sizeof(PTE) == sizeof(PVOID), "Size mismatch, only 64-bit supported.");

// TODO: update for X64
typedef struct _PTE_ENTRY
{
    ULONG Present : 1;
    ULONG Write : 1;
    ULONG Owner : 1; // E.g., user mode or supervisor mode
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Dirty : 1;
    ULONG PAT : 1;
    ULONG Global : 1;
    ULONG CopyOnWrite : 1;
    ULONG Prototype : 1;
    ULONG Transition : 1;
    ULONG Address : 20;
} PTE_ENTRY;

// TODO: update for X64
typedef struct _PDE_ENTRY
{
    ULONG Present : 1;
    ULONG Write : 1;
    ULONG Owner : 1;
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Reserved1 : 1;
    ULONG PageSize : 1;
    ULONG Global : 1;
    ULONG Reserved : 3;
    ULONG Address : 20;
} PDE_ENTRY;

// See Table 24-8. Format of Extended-Page-Table Pointer
typedef union _EPTP {
    ULONG64 All;
    struct
    {
        UINT64 MemoryType : 3;            // bit 2:0 (0 = Uncacheable (UC) - 6 = Write - back(WB))
        UINT64 PageWalkLength : 3;        // bit 5:3 (This value is 1 less than the EPT page-walk length)
        UINT64 DirtyAndAceessEnabled : 1; // bit 6  (Setting this control to 1 enables accessed and dirty flags for EPT)
        UINT64 Reserved1 : 5;             // bit 11:7
        UINT64 PML4Address : 36;
        UINT64 Reserved2 : 16;
    } Fields;
} EPTP, *PEPTP;

// See Table 28-1.
typedef union _EPT_PML4E {
    ULONG64 All;
    struct
    {
        UINT64 Read : 1;               // bit 0
        UINT64 Write : 1;              // bit 1
        UINT64 Execute : 1;            // bit 2
        UINT64 Reserved1 : 5;          // bit 7:3 (Must be Zero)
        UINT64 Accessed : 1;           // bit 8
        UINT64 Ignored1 : 1;           // bit 9
        UINT64 ExecuteForUserMode : 1; // bit 10
        UINT64 Ignored2 : 1;           // bit 11
        UINT64 PhysicalAddress : 36;   // bit (N-1):12 or Page-Frame-Number
        UINT64 Reserved2 : 4;          // bit 51:N
        UINT64 Ignored3 : 12;          // bit 63:52
    } Fields;
} EPT_PML4E, *PEPT_PML4E;

// See Table 28-3
typedef union _EPT_PDPTE {
    ULONG64 All;
    struct
    {
        UINT64 Read : 1;               // bit 0
        UINT64 Write : 1;              // bit 1
        UINT64 Execute : 1;            // bit 2
        UINT64 Reserved1 : 5;          // bit 7:3 (Must be Zero)
        UINT64 Accessed : 1;           // bit 8
        UINT64 Ignored1 : 1;           // bit 9
        UINT64 ExecuteForUserMode : 1; // bit 10
        UINT64 Ignored2 : 1;           // bit 11
        UINT64 PhysicalAddress : 36;   // bit (N-1):12 or Page-Frame-Number
        UINT64 Reserved2 : 4;          // bit 51:N
        UINT64 Ignored3 : 12;          // bit 63:52
    } Fields;
} EPT_PDPTE, *PEPT_PDPTE;

// See Table 28-5
typedef union _EPT_PDE {
    ULONG64 All;
    struct
    {
        UINT64 Read : 1;               // bit 0
        UINT64 Write : 1;              // bit 1
        UINT64 Execute : 1;            // bit 2
        UINT64 Reserved1 : 5;          // bit 7:3 (Must be Zero)
        UINT64 Accessed : 1;           // bit 8
        UINT64 Ignored1 : 1;           // bit 9
        UINT64 ExecuteForUserMode : 1; // bit 10
        UINT64 Ignored2 : 1;           // bit 11
        UINT64 PhysicalAddress : 36;   // bit (N-1):12 or Page-Frame-Number
        UINT64 Reserved2 : 4;          // bit 51:N
        UINT64 Ignored3 : 12;          // bit 63:52
    } Fields;
} EPT_PDE, *PEPT_PDE;

// See Table 28-6
typedef union _EPT_PTE {
    ULONG64 All;
    struct
    {
        UINT64 Read : 1;               // bit 0
        UINT64 Write : 1;              // bit 1
        UINT64 Execute : 1;            // bit 2
        UINT64 EPTMemoryType : 3;      // bit 5:3 (EPT Memory type)
        UINT64 IgnorePAT : 1;          // bit 6
        UINT64 Ignored1 : 1;           // bit 7
        UINT64 AccessedFlag : 1;       // bit 8
        UINT64 DirtyFlag : 1;          // bit 9
        UINT64 ExecuteForUserMode : 1; // bit 10
        UINT64 Ignored2 : 1;           // bit 11
        UINT64 PhysicalAddress : 36;   // bit (N-1):12 or Page-Frame-Number
        UINT64 Reserved : 4;           // bit 51:N
        UINT64 Ignored3 : 11;          // bit 62:52
        UINT64 SuppressVE : 1;         // bit 63
    } Fields;
} EPT_PTE, *PEPT_PTE;