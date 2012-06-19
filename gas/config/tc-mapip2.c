#include "as.h"
#include "struc-symbol.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "opcodes/mapip2-desc.h"
#include "opcodes/mapip2-opc.h"
#include "cgen.h"

typedef struct mapip2_insn
{
  const CGEN_INSN *	insn;
  const CGEN_INSN *	orig_insn;
  CGEN_FIELDS		fields;
#if CGEN_INT_INSN_P
  CGEN_INSN_INT         buffer [1];
#define INSN_VALUE(buf) (*(buf))
#else
  unsigned char         buffer [CGEN_MAX_INSN_SIZE];
#define INSN_VALUE(buf) (buf)
#endif
  char *		addr;
  fragS *		frag;
  int                   num_fixups;
  fixS *                fixups [GAS_CGEN_MAX_FIXUPS];
  int                   indices [MAX_OPERAND_INSTANCES];
} mapip2_insn;

//******************************************************************************
// special characters
//******************************************************************************

/* This array holds the chars that always start a comment.  If the
	pre-processor is disabled, these aren't very useful.  */
const char mapip2_comment_chars[] = "//";

/* This array holds the chars that only start a comment at the beginning of
	a line.  If the line seems to have the form '# 123 filename'
	.line and .file directives will appear in the pre-processed output.  */
/* Note that input_file.c hand checks for '#' at the beginning of the
	first line of the input file.  This is because the compiler outputs
	#NO_APP at the beginning of its output.  */
/* Also note that comments started like this one will always
	work if '/' isn't otherwise defined.  */
const char line_comment_chars[] = "//";

const char line_separator_chars[] = ";";

/* Chars that can be used to separate mant from exp in floating point
	nums.  */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant.
	As in 0f12.456
	or    0d1.2345e12  */
const char FLT_CHARS[] = "rRsSfFdDxXpP";


//******************************************************************************
// options
//******************************************************************************

const char *md_shortopts = "";

enum Option {
	OPTION_DATASIZE = OPTION_MD_BASE,
	OPTION_STACKSIZE,
	OPTION_HEAPSIZE,
	OPTION_BUILD_ID,
};

struct option md_longopts[] = {
  {"datasize", required_argument, NULL, OPTION_DATASIZE},
  {"stacksize", required_argument, NULL, OPTION_STACKSIZE},
  {"heapsize", required_argument, NULL, OPTION_HEAPSIZE},
  {"buildid", required_argument, NULL, OPTION_BUILD_ID},
  {NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);

/* md_parse_option
 *	Invocation line includes a switch not recognized by the base assembler.
 *	See if it's a processor-specific option.
 */
int md_parse_option(int o, const char* arg ATTRIBUTE_UNUSED)
{
	switch(o) {
	case OPTION_DATASIZE:
	case OPTION_STACKSIZE:
	case OPTION_HEAPSIZE:
	case OPTION_BUILD_ID:
		as_fatal (_("Unimplemented option."));
	default:
		return 0;
	}
	return 1;
}

void md_show_usage(FILE* stream)
{
	fprintf(stream, "mapip2 options: todo\n");
}

void md_begin(void)
{
	/* Initialize the `cgen' interface.  */

	/* Set the machine number and endian.  */
	gas_cgen_cpu_desc = mapip2_cgen_cpu_open (CGEN_CPU_OPEN_MACHS, 0,
		CGEN_CPU_OPEN_ENDIAN,
		CGEN_ENDIAN_LITTLE,
		CGEN_CPU_OPEN_END);
	mapip2_cgen_init_asm (gas_cgen_cpu_desc);

	/* This is a callback from cgen to gas to parse operands.  */
	cgen_set_parse_operand_fn (gas_cgen_cpu_desc, gas_cgen_parse_operand);
}

void md_number_to_chars(char* buf, valueT val, int n)
{
	number_to_chars_littleendian (buf, val, n);
}

void md_operand(expressionS* exp ATTRIBUTE_UNUSED)
{
	// todo?
}

/* This table describes all the machine specific pseudo-ops the assembler
	has to support.  The fields are:
	pseudo-op name without dot
	function to call to execute this pseudo-op
	Integer arg to pass to the function. */

const pseudo_typeS md_pseudo_table[] =
{
	{ 0, 0, 0 }
};

symbolS* md_undefined_symbol(char* name ATTRIBUTE_UNUSED)
{
	return NULL;
}

char* md_atof(int type, char* litP, int* sizeP)
{
	return ieee_md_atof(type, litP, sizeP, TRUE);
}

void md_assemble (char * str)
{
	mapip2_insn insn;
	char* errmsg;

	/* Initialize GAS's cgen interface for a new instruction.  */
	gas_cgen_init_parse();

	insn.insn = mapip2_cgen_assemble_insn(
		gas_cgen_cpu_desc, str, &insn.fields, insn.buffer, &errmsg);

	if (!insn.insn)
	{
		as_bad("%s", errmsg);
		return;
	}

	/* Doesn't really matter what we pass for RELAX_P here.  */
	gas_cgen_finish_insn (insn.insn, insn.buffer,
		CGEN_FIELDS_BITSIZE(& insn.fields), 1, NULL);
}


/* Return the bfd reloc type for OPERAND of INSN at fixup FIXP.
   Returns BFD_RELOC_NONE if no reloc type can be found.
   *FIXP may be modified if desired.  */

bfd_reloc_code_real_type
md_cgen_lookup_reloc (const CGEN_INSN * insn ATTRIBUTE_UNUSED,
	const CGEN_OPERAND * operand,
	fixS * fixP)
{
	switch (operand->type)
	{
	case MAPIP2_OPERAND_IMM1:
	case MAPIP2_OPERAND_IMM2:
	case MAPIP2_OPERAND_AIADDR1:
	case MAPIP2_OPERAND_ADADDR1:
	case MAPIP2_OPERAND_AIADDR2:
	case MAPIP2_OPERAND_ADADDR2:
	case MAPIP2_OPERAND_SIMM1:
	case MAPIP2_OPERAND_SIMM2:
		fixP->fx_pcrel = 0;
		return BFD_RELOC_32;

	case MAPIP2_OPERAND_RIADDR1:
	case MAPIP2_OPERAND_RIADDR2:
		fixP->fx_pcrel = 1;
		return BFD_RELOC_32;

	case MAPIP2_OPERAND_PC:
	case MAPIP2_OPERAND_RD:
	case MAPIP2_OPERAND_RS:
	case MAPIP2_OPERAND_MAX:
	case MAPIP2_OPERAND_IMM08:
		return BFD_RELOC_NONE;
	}
	abort();
}

/* *fragP has been relaxed to its final size, and now needs to have
   the bytes inside it modified to conform to the new size.

   Called after relaxation is finished.
   fragP->fr_type == rs_machine_dependent.
   fragP->fr_subtype is the subtype of what the address relaxed to.  */

void
md_convert_frag (bfd *   abfd ATTRIBUTE_UNUSED,
		 segT    sec  ATTRIBUTE_UNUSED,
		 fragS * fragP ATTRIBUTE_UNUSED)
{
	/* FIXME */
}

valueT
md_section_align (segT segment, valueT size)
{
	int align = bfd_get_section_alignment (stdoutput, segment);
	return ((size + (1 << align) - 1) & (-1 << align));
}

bfd_boolean
mapip2_fix_adjustable (struct fix* fixP)
{
	/* We need the symbol name for the VTABLE entries.  */
	if (fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT
			|| fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
		return 0;

	return 1;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from_section (fixS * fixP, segT sec)
{
	if (fixP->fx_addsy != (symbolS *) NULL
			&& (! S_IS_DEFINED (fixP->fx_addsy)
		|| S_GET_SEGMENT (fixP->fx_addsy) != sec))
		/* The symbol is undefined (or is defined but not in this section).
			Let the linker figure it out.  */
		return 0;

	return (fixP->fx_frag->fr_address + fixP->fx_where) & ~1;
}

/* Interface to relax_segment.  */

/* FIXME: Look through this.  */

const relax_typeS md_relax_table[] =
{
/* The fields are:
   1) most positive reach of this state,
   2) most negative reach of this state,
   3) how many bytes this mode will add to the size of the current frag
   4) which index into the table to try if we can't fit into this one.  */

  /* The first entry must be unused because an `rlx_more' value of zero ends
     each list.  */
  {1, 1, 0, 0},
};


/* Return an initial guess of the length by which a fragment must grow to
	hold a branch to reach its destination.
	Also updates fr_type/fr_subtype as necessary.

	Called just before doing relaxation.
	Any symbol that is now undefined will not become defined.
	The guess for fr_var is ACTUALLY the growth beyond fr_fix.
	Whatever we do to grow fr_fix or fr_var contributes to our returned value.
	Although it may not be explicit in the frag, pretend fr_var starts with a
	0 value.  */

int
md_estimate_size_before_relax (fragS * fragP, segT segment)
{
	/* The only thing we have to handle here are symbols outside of the
		current segment.  They may be undefined or in a different segment in
		which case linker scripts may place them anywhere.
		However, we can't finish the fragment here and emit the reloc as insn
		alignment requirements may move the insn about.  */

	if (S_GET_SEGMENT (fragP->fr_symbol) != segment)
	{
		/* The symbol is undefined in this segment.
			Change the relaxation subtype to the max allowable and leave
			all further handling to md_convert_frag.  */
		fragP->fr_subtype = 2;

		{
			const CGEN_INSN * insn;
			int i;

			/* Update the recorded insn.
				 Fortunately we don't have to look very far.
				 FIXME: Change this to record in the instruction the next higher
				 relaxable insn to use.  */
			for (i = 0, insn = fragP->fr_cgen.insn; i < 4; i++, insn++)
			{
				if ((strcmp (CGEN_INSN_MNEMONIC (insn),
					CGEN_INSN_MNEMONIC (fragP->fr_cgen.insn))
					== 0)
					&& CGEN_INSN_ATTR_VALUE (insn, CGEN_INSN_RELAXED))
					break;
			}
			if (i == 4)
				abort ();

			fragP->fr_cgen.insn = insn;
			return 2;
		}
	}

	return md_relax_table[fragP->fr_subtype].rlx_length;
}

unsigned long mapip2_machine = 0; /* default */
