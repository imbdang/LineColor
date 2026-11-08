ENTRY(Reset_Handler)

MEMORY
{
  FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 64K
  SRAM(rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Stack MSP cho ISR va main truoc khi scheduler chay; stack task cap phat tinh trong .bss */
_Min_Msp_Size = 0x400;

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > SRAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > SRAM

  ASSERT(_ebss + _Min_Msp_Size <= ORIGIN(SRAM) + LENGTH(SRAM), "SRAM khong du cho stack MSP")
}
