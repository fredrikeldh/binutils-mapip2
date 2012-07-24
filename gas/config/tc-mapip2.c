#include "as.h"
#include "struc-symbol.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "opcodes/mapip2-desc.h"
#include "opcodes/mapip2-gen-opcodes.h"
#include <errno.h>

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

/***********************************************************************/
/* md_assemble and friends */
/***********************************************************************/

static int isOperandEnd(char c) {
	return c == 0 || c == ',' || c == ']' || c == '+' || c == '-';
}

typedef struct mapip2_data {
	char* buf;
	int length;	// length of instruction, in bytes.
	// offset from start of instruction bytecode to any symbol reference,
	// as expressed by a fixup.
	// there is max one symbol ref per instruction.
	int fixOffset;
	int regOffset;	// offset in buf to next register. start at 1, ++ when writing.
	char* str;
	const mapip2_parse_node* children;
	int nc_op;
	int level;
	const char* symbol;
	char* symbolEnd;
} mapip2_data;

// returns zero on failure, non-zero on success.
typedef int (*parseFunc)(mapip2_data* data);

#define DEBUG_LEVEL 1

// operand parse assert
#define OP_ASSERT(c) if(*data->str != (c)) { fprintf(stderr, "Expected %c at %s on line %i\n", c, data->str, __LINE__);\
	as_bad (_("Illegal operand form.")); return 0; }

#define DUMP if(DEBUG_LEVEL >= 3) fprintf(stderr, "%s(%s)\n", __FUNCTION__, data->str)

#define INSN_LEN(a) if(data->length < (a)) data->length = a

static void setConstant(mapip2_data* data, int c) {
	gas_assert(data->fixOffset >= 1);
	*(int*)&data->buf[data->fixOffset] = c;
	INSN_LEN(data->fixOffset + 4);
}

static void setConstantLong(mapip2_data* data, long long c) {
	gas_assert(data->fixOffset >= 1);
	*(long long*)&data->buf[data->fixOffset] = c;
	INSN_LEN(data->fixOffset + 8);
}

static void setConstant8(mapip2_data* data, int c) {
	gas_assert(data->fixOffset >= 1);
	gas_assert(c >= 0 && c < 0xff);
	data->buf[data->fixOffset] = c;
	INSN_LEN(data->fixOffset + 1);
}

static void setRegister(mapip2_data* data, char c) {
	data->buf[data->regOffset++] = c;
	INSN_LEN(data->regOffset);
}

static const mapip2_parse_node* findOperandNode(const mapip2_parse_node* children, int nc, mapip2_operand op) {
	gas_assert(children);
	gas_assert(nc > 0);
	for(int i=0; i<nc; i++) {
		if(children[i].operand == op)
			return &children[i];
	}
	return 0;
}

typedef void (*strtolFunc)(const char* str, char** endptr, int base, void* res);

static int parseConstantBase(mapip2_data* data, void* res, strtolFunc stf) {
	DUMP;
	if(!ISDIGIT(*data->str) && *data->str != '-')
		return 0;
	char* str = data->str;
	int base = 10;
	if(*str == '0') {	// octal or hex
		str++;
		base = 8;
		if(*str == 'x') {	// hex
			str++;
			base = 16;
		}
	}
	errno = 0;
	char* endptr;
	stf(str, &endptr, base, res);
	if(errno != 0) {
		fprintf(stderr, "Could not parse immediate: %s\n", strerror(errno));
		return 0;
	}
	data->str = endptr;
	return 1;
}

static void strtolInt(const char* str, char** endptr, int base, void* res)
{
	*(int*)res = strtol(str, endptr, base);
}

static void strtolLong(const char* str, char** endptr, int base, void* res)
{
	*(long long*)res = strtoll(str, endptr, base);
}

static int parseConstantBaseInt(mapip2_data* data, int* res) {
	if(data->str[0] == '0' && isOperandEnd(data->str[1]))
	{
		data->str++;
		*res = 0;
		return 1;
	}
	return parseConstantBase(data, res, strtolInt);
}

static int parseConstantBaseLong(mapip2_data* data, long long* res) {
	if(data->str[0] == '0' && isOperandEnd(data->str[1]))
	{
		data->str++;
		*res = 0;
		return 1;
	}
	return parseConstantBase(data, res, strtolLong);
}

static int parseConstant(mapip2_data* data) {
	int i;
	if(!parseConstantBaseInt(data, &i))
		return 0;
	setConstant(data, i);
	return 1;
}

static int parseConstantDouble(mapip2_data* data) {
	long long ll;
	if(!parseConstantBaseLong(data, &ll))
		return 0;
	setConstantLong(data, ll);
	return 1;
}

static int parseConstant8(mapip2_data* data) {
	int i;
	if(!parseConstantBaseInt(data, &i))
		return 0;
	setConstant8(data, i);
	return 1;
}

static int parseSymbol(mapip2_data* data) {
	DUMP;
	char* str = data->str;
	const char* start = str;
	// _ and alpha are valid start chars.
	if(!(*str == '_' || ISALPHA(*str)))
		return 0;
	str++;
	// , or NULL may end a symbol.
	while(!isOperandEnd(*str)) {
		// _ . and alnum are valid chars.
		if(!(*str == '_' || *str == '.' || ISALNUM(*str)))
			return 0;
		str++;
	}
	// at this point, we need to be destructive.
	data->str = str;

	// we need to set up a relocation entry.
	gas_assert(data->fixOffset >= 1);
	data->symbol = start;
	data->symbolEnd = str;
	INSN_LEN(data->fixOffset + 4);

	return 1;
}

// where imm is one of:
// &sym, &sym+constant, constant
static int parseImm(mapip2_data* data) {
	DUMP;
	if(*data->str == '&') {
		data->str++;
		if(!parseSymbol(data))
			return 0;
		// &sym alone
		if(*data->str == ']' || *data->str == 0)
			return 1;
		// allow &sym - constant
		if(*data->str != '-') {
			OP_ASSERT('+');
			data->str++;
		}
	}
	// constant
	return parseConstant(data);
}

static int parseImmAllowRawSymbol(mapip2_data* data) {
	int res = parseImm(data);
	if(res)
		return res;
	char c = *data->str;
	if(c == 'L' || c == '_')	// a symbol must start with either L or _.
		return parseSymbol(data);
	else
		return 0;
}

static int parseRegister(mapip2_data* data) {
	DUMP;
	char* str = data->str;
	// find end of operand
	while(!isOperandEnd(*str))
		str++;
	int len = str - data->str;
	for(size_t i=0; i<mapip2_register_name_count; i++) {
		const char* n = mapip2_register_names[i];
		//fprintf(stderr, "test(%zi, %s)\n", len, n);
		if(strncmp(data->str, n, len) == 0) {
			data->str = str;
			setRegister(data, (char)i);
			return 1;
		}
	}
	return 0;
}

static int parseFloatRegister(mapip2_data* data) {
	DUMP;
	char* str = data->str;
	// find end of operand
	while(!isOperandEnd(*str))
		str++;
	int len = str - data->str;
	for(size_t i=0; i<mapip2_float_register_name_count; i++) {
		const char* n = mapip2_float_register_names[i];
		//fprintf(stderr, "test(%zi, %s)\n", len, n);
		if(strncmp(data->str, n, len) == 0) {
			data->str = str;
			setRegister(data, (char)i);
			return 1;
		}
	}
	return 0;
}

// We support many varieties:
// [reg], [imm], [reg,imm], [reg,sym] (without &)
// where imm is one of:
// &sym, &sym+constant, constant
// They all result in reg + immediate.
static int parseAddr(mapip2_data* data) {
	DUMP;
	if(parseImm(data)) {	// [imm]
		setRegister(data, 0);	//zr
		OP_ASSERT(']');
		data->str++;
		return 1;
	}
	if(!parseRegister(data))
		return 0;
	if(*data->str == ']') {
		data->str++;
		setConstant(data, 0);
		return 1;
	}
	OP_ASSERT(',');
	data->str++;
	if(!parseImmAllowRawSymbol(data))
		return 0;
	OP_ASSERT(']');
	data->str++;
	return 1;
}

// attempts to parse an instruction's operands.
// store the result in buf.
// returns zero on failure, non-zero on success.
static int try_assemble(mapip2_data* data)
{
	DUMP;
	char* buf = data->buf;
	char* str = data->str;
	const mapip2_parse_node* children = data->children;
	int nc_op = data->nc_op;

	switch(data->level) {
	case 1:
	case 2: if(data->fixOffset < data->level) data->fixOffset = data->level; break;
	case 3: data->fixOffset = 3; break;
	}

	// if there are no children left, we must be at the end of the instruction.
	if(!children) {
		if(*str == 0) {
			buf[0] = nc_op;
			INSN_LEN(1);
			return 1;
		}
		fprintf(stderr, "Too many operands: %s\n", str);
		return 0;
	}
	// otherwise, find a node to match the present operand.
	const mapip2_parse_node* node;
	parseFunc func;
	if(*str == '#') {	// immediate (0x hex, 0 octal, or decimal)
		node = findOperandNode(children, nc_op, IMM);
		func = parseConstant;
		if(!node) {
			node = findOperandNode(children, nc_op, FIMMS);
			func = parseConstant;
		}
		if(!node) {
			node = findOperandNode(children, nc_op, FIMMD);
			func = parseConstantDouble;
		}
		if(!node) {
			node = findOperandNode(children, nc_op, IMM8);
			func = parseConstant8;
		}
	} else if(*str == '[') {	// data address
		node = findOperandNode(children, nc_op, ADADDR);
		data->fixOffset = 3;
		func = parseAddr;
	} else if(*str == '&') {	// symbol (can be imm or instruction address)
		data->str--;
		func = parseImm;
		node = findOperandNode(children, nc_op, AIADDR);
		if(!node)
			node = findOperandNode(children, nc_op, RIADDR);
		if(!node)
			node = findOperandNode(children, nc_op, IMM);
	} else if(ISDIGIT(*str)) {	// syscall number
		data->str--;
		func = parseConstant8;
		node = findOperandNode(children, nc_op, IMM8);
	} else if(parseRegister(data)) {	// register
		func = NULL;
		if(DEBUG_LEVEL >= 3)
			fprintf(stderr, "Found register. matching operand %i(%s)\n",
				data->regOffset, data->regOffset == 2 ? "RD" : "RS");
		node = findOperandNode(children, nc_op, data->regOffset == 2 ? RD : RS);
	} else if(parseFloatRegister(data)) {	// float register
		func = NULL;
		if(DEBUG_LEVEL >= 3)
			fprintf(stderr, "Found register. matching operand %i(%s)\n",
				data->regOffset, data->regOffset == 2 ? "FRD" : "FRS");
		node = findOperandNode(children, nc_op, data->regOffset == 2 ? FRD : FRS);
	} else {
		fprintf(stderr, "Invalid operand: %s\n", str);
		return 0;
	}
	if(!node) {
		fprintf(stderr, "Disallowed operand for this mnemonic: %s\n", str);
		fprintf(stderr, "%i allowed operands.\n", nc_op);
		return 0;
	}
	// and continue parsing.
	if(func) {
		data->str++;
		if(!func(data))
			return 0;
	}
	if(*data->str == ',')
		data->str++;
	data->children = node->children;
	data->nc_op = node->nc_op;
	data->level++;
	return try_assemble(data);
}

/* Hard to tell what this is supposed to do.
* I'm guessing one can use frag_more() to get a pointer to the output buffer, then write to it.
* Presumably it will be flushed when this function returns.
*/
void md_assemble(char* str)
{
	if(DEBUG_LEVEL >= 2)
		fprintf(stderr, "md_assemble(%s)\n", str);
	char buf[16];	// more than enough to contain any mapip2 instruction.
	for(size_t i=0; i<mapip2_mnemonic_count; i++) {
		const mapip2_mnemonic* m = &mapip2_mnemonics[i];
		int mLen = strlen(m->mnemonic);
		// we rely on the rest of the assembler to make sure there are no linebreaks in str.
		if(strncmp(str, m->mnemonic, mLen) == 0 && (ISSPACE(str[mLen]) || str[mLen] == 0)) {
			int pos = mLen;
			while(ISSPACE(str[pos]))
				pos++;
			mapip2_data data = { buf, 0, 0, 1, str + pos, m->children, m->nc_op, 1, NULL, NULL };
			if(!try_assemble(&data)) {
				fprintf(stderr, "insn: %s\n", str);
				fprintf(stderr, "error: %s\n", data.str);
				as_fatal (_("Illegal instruction form."));
			}
			if(DEBUG_LEVEL >= 2)
				fprintf(stderr, "length: %i\n", data.length);
			gas_assert(*data.str == 0);
			gas_assert(data.length > 0);
			char* fm = frag_more(data.length);
			memcpy(fm, buf, data.length);
			// fix_new must not be called before frag_more, lest we write outside the frag.
			if(data.symbol) {
				// number to be added to the symbol value.
				int addition = *(int*)(buf + data.length - 4);
				*data.symbolEnd = 0;
				fix_new(frag_now, frag_now_fix() - 4, 4, symbol_find_or_make(data.symbol),
					addition, FALSE, BFD_RELOC_32);
			}
			return;
		}
	}
	as_fatal (_("Unknown instruction."));
}

void mapip2_md_apply_fix(fixS* fixp, valueT* v, segT s) {
#if 0	// doesn't seem to be needed.
	as_bad_where (fixp->fx_file, fixp->fx_line,
		_("mapip2_md_apply_fix"));
#endif
}

arelent* mapip2_tc_gen_reloc(asection* a, fixS* fixp) {
	arelent *rel;
	bfd_reloc_code_real_type r_type;

	if (fixp->fx_addsy && fixp->fx_subsy)
	{
		if ((S_GET_SEGMENT (fixp->fx_addsy) != S_GET_SEGMENT (fixp->fx_subsy))
			|| S_GET_SEGMENT (fixp->fx_addsy) == undefined_section)
		{
			as_bad_where (fixp->fx_file, fixp->fx_line,
				_("Difference of symbols in different sections is not supported"));
			return NULL;
		}
	}

	rel = xmalloc (sizeof (arelent));
	rel->sym_ptr_ptr = xmalloc (sizeof (asymbol *));
	*rel->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
	rel->address = fixp->fx_frag->fr_address + fixp->fx_where;
	rel->addend = fixp->fx_offset;

	r_type = fixp->fx_r_type;

#if DEBUG
	fprintf (stderr, "%s\n", bfd_get_reloc_code_name (r_type));
	fflush (stderr);
#endif

	rel->howto = bfd_reloc_type_lookup (stdoutput, r_type);
	if (rel->howto == NULL)
	{
		as_bad_where (fixp->fx_file, fixp->fx_line,
			_("Cannot represent relocation type %s"),
		bfd_get_reloc_code_name (r_type));
		return NULL;
	}

	return rel;
}

#if 0
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
	case MAPIP2_OPERAND_IMM:
	case MAPIP2_OPERAND_AIADDR:
	case MAPIP2_OPERAND_ADADDR:
	case MAPIP2_OPERAND_SIMM:
		fixP->fx_pcrel = 0;
		return BFD_RELOC_32;

	case MAPIP2_OPERAND_RIADDR:
		fixP->fx_pcrel = 1;
		return BFD_RELOC_32;

	case MAPIP2_OPERAND_PC:
	case MAPIP2_OPERAND_RD:
	case MAPIP2_OPERAND_RS:
	case MAPIP2_OPERAND_IMM08:
		return BFD_RELOC_NONE;
	}
	abort();
}
#endif

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

#if 0//		{
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
#endif//		}
	}

	return md_relax_table[fragP->fr_subtype].rlx_length;
}

unsigned long mapip2_machine = 0; /* default */
