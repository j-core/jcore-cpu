/**************************************
 SuperH (SH-2) C Compiler Linker Script
 **************************************/ 

OUTPUT_FORMAT("elf32-sh")
OUTPUT_ARCH(sh)

MEMORY
{
	ram    : o = 0x00000000, l = 0x7b00
	stack  : o = 0x00007d00, l = 0x0300
}

SECTIONS 				
{
.text :	{
	*(.vect)
	*(.text) 				
	*(.strings)
   	 _etext = . ; 
	}  > ram

.tors : {
	___ctors = . ;
	*(.ctors)
	___ctors_end = . ;
	___dtors = . ;
	*(.dtors)
	___dtors_end = . ;
	}  > ram

.rodata : {
    *(.rodata*)
    } >ram

__idata_start = ADDR(.text) + SIZEOF(.text) + SIZEOF(.tors) + SIZEOF(.rodata); 
.data : AT(__idata_start) {
	__idata_start = .;
        _sdata = . ;
	*(.data)
	_edata = . ;
	}  > ram
__idata_end = __idata_start + SIZEOF(.data);

.bss : {
	_bss_start = .;
	*(.bss)
	*(COMMON)
	_end = .;
	}  >ram

.stack :
	{
	_stack = .;
	*(.stack)
	} > stack
}
