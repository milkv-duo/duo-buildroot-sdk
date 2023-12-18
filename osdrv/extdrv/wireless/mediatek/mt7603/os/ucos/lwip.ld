/* OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips", "elf32-littlemips") */
OUTPUT_ARCH(mips)

/**** Start point ****/
ENTRY(kernel_entry)			/* Entry point of application		*/


SECTIONS
{

  /**** Code and read-only data ****/
									  
  /* Reserve 0x1000(4K) for Vectored Interrupts (Offset: 0x100) */										
  .text 0x88001000 :
  {
    _ftext = .;	/* Text and read-only data */ 
    head.o (.text) 		/* Entry point	*/
    *(.text)
	*(.text.*)
    _etext = .;	/* End of text section */ 
  }

  /* read only data */
    . = ALIGN(32);
  .rodata : { *(.rodata) }

  /* writeable */
  .data :		/* Data */
  {
    _fdata = .;	/* Start of initialised data		*/
    *(.data)
	*(.data.*)
  }

	  
  . = ALIGN(32);
  _gp = . + 0x8000; /* Base of small data			*/
  .lit8 : { *(.lit8) }
  .lit4 : { *(.lit4) }
  /* We want the small data sections together, so single-instruction offsets 
     can access them all, and initialized data all before uninitialized, so  
     we can shorten the on-disk segment size.  */                            
  .sdata     : { *(.sdata) } 

  . = ALIGN(32);
  .data.cacheline_aligned : { *(.data.cacheline_aligned) }

  _edata  = .;	/* End of data section */

                                                      
  /* will be freed after init */                                            
/*  . = ALIGN(4096);*/		/* Init code and data */                            
  . = ALIGN(16);
  __init_begin = .;                                                         
  .init.text : {                                                            
	_sinittext = .;                                                         
	*(.init.text)
	*(.init.text.*)
	_einittext = .;                                                         
  }
  .init.data : { 
  	*(.init.data) 
	*(.init.data.*) 
  }

  . = ALIGN(32);                                                            
  __setup_start = .;                                                        
  .init.setup : { *(.init.setup) } 
  __setup_end = .;                                                          
                                                                            
  .early_initcall.init : {                                                  
  __earlyinitcall_start = .;                                                
	*(.initcall.early1.init)                                                
  }
  __earlyinitcall_end = .;                                                  
                                                                            
  __initcall_start = .;                                                     
  .initcall.init : {                                                        
	*(.initcall1.init)                                                      
	*(.initcall2.init)                                                      
	*(.initcall3.init)                                                      
	*(.initcall4.init)                                                      
	*(.initcall5.init)                                                      
	*(.initcall6.init)                                                      
	*(.initcall7.init)                                                      
  }                                                                         
  __initcall_end = .;   
/*  . = ALIGN(4096); */
  . = ALIGN(32);
  __init_end = .;                                            
  /* freed after init ends here */                           
  
  . = ALIGN(16);
  .symtbl : {   
  	__symtbl_start = .;
	KEEP(*(.symtbl.data))
     /* *(.symtbl.data) */
    __symtbl_end = .;
  }

  .esram.data  0x80800000 : 
  AT (__symtbl_end)
  {
  	_esram_data_start = .;
  	*(.esram.data)
	*(.esram.data.*)
	 _esram_data_end = .;
  }
   
  . = ABSOLUTE(__symtbl_end) + SIZEOF(.esram.data); 
  
  /* uninitialised data */
  __bss_start = .;	 /* BSS */

  .bss : 
  {
    *(.bss)
	*(.bss.*)
    *(COMMON)
  }
	
  __bss_stop = .;

 
  /* top of free memory */
  _end = .;

  

  /* Sections to be discarded */
  /DISCARD/ : {
    	*(.sym.text)
        *(.exit.text)
        *(.exit.data)
        *(.exitcall.exit)

	/* ABI crap starts here */
	*(.comment)
	*(.MIPS.options)
	*(.note)
	*(.options)
	*(.pdr)
	*(.reginfo)
	*(.mdebug*)
  }




  /* This is the MIPS specific mdebug section.  */
  .mdebug : { *(.mdebug) }
  /* These are needed for ELF backends which have not yet been
     converted to the new style linker.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  /* DWARF debug sections.
     Symbols in the .debug DWARF section are relative to the beginning of the
     section so we begin .debug at 0.  It's not clear yet what needs to happen
     for the others.   */
  .debug          0 : { *(.debug) }
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  .line           0 : { *(.line) }
  /* These must appear regardless of  .  */
  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .comment : { *(.comment) }
  .note : { *(.note) }

}

