OUTPUT_FORMAT(binary)
ENTRY(entry)
OUTPUT_ARCH(i386)

SECTIONS
{
   . = 0;
    
   .text :
    {
        *(.entry)
        *(.text)
    }
    
   .data :
    {
        *(.data)
    }
    
   .stack :
    {
       . =. + 0x200;
    }
    
    /DISCARD/ :
    {
        *(*)
    }
}