ENTRY(start)
phys = 0x1000;
program_addr_phys = 0x3003000;
program_addr_virt = 0xF00000;
SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    EXCLUDE_FILE (*test_program.o) *(.text)
    EXCLUDE_FILE (*test_program.o) *(.rodata)
    . = ALIGN(4096);
  }
  .data : AT(phys + (data - code))
  {
    data = .;
    EXCLUDE_FILE (*test_program.o) *(.data)
    . = ALIGN(4096);
  }
  .bss : AT(phys + (bss - code))
  {
    bss = .;
    EXCLUDE_FILE (*test_program.o)  *(.bss)
    . = ALIGN(4096);
  }
 
  .program 0xF00000 : AT(0x3003000) {
    program = .;
    *(.text);
    *(.rodata);
    *(.data);
    *(.bss);
  }

  end = .;
}