ENTRY(start)
phys = 0x1000;
program_addr_phys = 0x3004000; /* this is 0x1000 higher... for some reason */
program_addr_virt = 0xF00000;
address_linker = (((ADDR (.bss) + SIZEOF (.bss) + 4096) & 0xFFFFFFFFF000) - 512);

SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    EXCLUDE_FILE (*"src/stock/test_program.o") *(.text)
    EXCLUDE_FILE (*"src/stock/test_program.o") *(.rodata)
    . = ALIGN(4096);
  }
  .data : AT(phys + (data - code))
  {
    data = .;
    EXCLUDE_FILE (*"src/stock/test_program.o") *(.data)
    . = ALIGN(4096);
  }
  .bss : AT(phys + (bss - code))
  {
    bss = .;
    EXCLUDE_FILE (*"src/stock/test_program.o")  *(.bss)
    . = ALIGN(4096);
  }
 
  .program program_addr_virt : AT(((ADDR (.bss) + SIZEOF (.bss) + 4096) & 0xFFFFFFFFF000) - 512)
  {
    program = .;
    *(.program_header)
    *(.file_functions)
    . = ALIGN(4096);
    length_linker = . - program;
  }

  end = .;
}