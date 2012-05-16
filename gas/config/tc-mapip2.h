
#define TC_MAPIP2

#define LISTING_HEADER "Mapip2 GAS "

/* The target BFD architecture.  */
#define TARGET_ARCH bfd_arch_mapip2

extern unsigned long mapip2_machine;
#define TARGET_MACH (mapip2_machine)

#define TARGET_FORMAT		"elf32-mapip2"
#define TARGET_BYTES_BIG_ENDIAN	0

extern const char mapip2_comment_chars [];
#define tc_comment_chars mapip2_comment_chars

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB	1

#define DIFF_EXPR_OK	1	/* .-foo gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

/* Values passed to md_apply_fix don't include the symbol value.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

#define md_apply_fix gas_cgen_md_apply_fix

extern bfd_boolean mapip2_fix_adjustable (struct fix *);
#define tc_fix_adjustable(FIX) mapip2_fix_adjustable (FIX)

#define tc_gen_reloc gas_cgen_tc_gen_reloc

/* Call md_pcrel_from_section(), not md_pcrel_from().  */
extern long md_pcrel_from_section (struct fix *, segT);
#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from_section (FIX, SEC)

/* For 8 vs 16 vs 32 bit branch selection.  */
extern const struct relax_type md_relax_table[];
#define TC_GENERIC_RELAX_TABLE md_relax_table
