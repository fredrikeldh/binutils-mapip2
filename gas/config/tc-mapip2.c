#include "as.h"
#include "struc-symbol.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "opcodes/mapip2-desc.h"
#include "opcodes/mapip2-opc.h"
#include "cgen.h"

//******************************************************************************
// special characters
//******************************************************************************

/* This array holds the chars that always start a comment.  If the
	pre-processor is disabled, these aren't very useful.  */
const char comment_chars[] = "#";

/* This array holds the chars that only start a comment at the beginning of
	a line.  If the line seems to have the form '# 123 filename'
	.line and .file directives will appear in the pre-processed output.  */
/* Note that input_file.c hand checks for '#' at the beginning of the
	first line of the input file.  This is because the compiler outputs
	#NO_APP at the beginning of its output.  */
/* Also note that comments started like this one will always
	work if '/' isn't otherwise defined.  */
const char line_comment_chars[] = "#";

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
int md_parse_option(int o, char* arg ATTRIBUTE_UNUSED)
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
	static int last_insn_had_delay_slot = 0;
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

	last_insn_had_delay_slot = CGEN_INSN_ATTR_VALUE(insn.insn, CGEN_INSN_DELAY_SLOT);
}
