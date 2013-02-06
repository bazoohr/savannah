/* ======================================== *
 * GNU linker script                        *
 * Hamid R. Bazoobandi (C) Copyright        *
 * 2009 Iran                                *
 * ======================================== *
 */
OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64",
	      "elf64-x86-64")
/* ======================================== */
OUTPUT_ARCH(i386:x86-64)
/* ======================================== */
ENTRY (_start);
/* ======================================== */
PHDRS
{
	text PT_LOAD;
	data PT_LOAD;
	rodata PT_LOAD;
	bss PT_LOAD;
}
/* ======================================== */
SECTIONS
{
	. = 0x40000000;
	.text :
		{
			*(.text)
			_etext = .;
		}:text
  . = ALIGN (0x200000);
	.data :
		{
			*(.data)
			_edata = .;
		}:data
  . = ALIGN (0x200000);
	.rodata :
		{
			*(.rodata)
			_erodata = .;
		}:rodata
  . = ALIGN (0x200000);
	.bss :
		{
			*(.bss)
			_ebss = .;
		}:bss
	/* ================================== */
	/DISCARD/ : {
		*(.note.GNU-stack) *(.gnu_debuglink)
		*(.stab)
	  	*(.stabstr)
		*(.stab.excl)
		*(.stab.exclstr)
		*(.stab.index)
		*(.stab.indexstr)
		*(.comment)
		*(.debug)
		*(.line)
		*(.debug_srcinfo)
		*(.debug_sfnames)
		*(.debug_aranges)
		*(.debug_pubnames)
		*(.debug_info .gnu.linkonce.wi.*)
		*(.debug_abbrev)
		*(.debug_line)
		*(.debug_frame)
		*(.debug_str)
		*(.debug_loc)
		*(.debug_macinfo)
		*(.debug_weaknames)
		*(.debug_funcnames)
		*(.debug_typenames)
		*(.debug_varnames)
		*(.debug_pubtypes)
		*(.debug_ranges)
		KEEP (*(.gnu.attributes))
	}
}
