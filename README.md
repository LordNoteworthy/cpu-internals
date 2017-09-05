# cpu-internals
Intel / AMD CPU Internals

- A VMM can use a different VMCS for each VM that it supports.
- For a VM with multiple LP (VP), the VMM can use a different VMCS for each VP.
- A VMCS region is a memory region assosiated with each VMCS by the LP.
- The VMCS pointer (64-bit) contain the physical address of the the VMCS region and should be aligned to a 4-KB boundary.

