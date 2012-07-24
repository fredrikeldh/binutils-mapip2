#include <stdlib.h>

typedef enum mapip2_operand {
	RD,	// register, destination
	RS,	// register, source
	IMM,	// immediate
	IMM8,	// 8-bit immediate
	ADADDR,	// absolute data address
	AIADDR,	// absolute instruction address
	RIADDR,	// relative instruction address
	FRD,	// float register, destination
	FRS,	// float resister, source
	FIMMS,	// float immediate
	FIMMD,	// double immediate
	END,
} mapip2_operand;

#define MAX_SYNTAX_ELEMENTS 4

typedef struct mapip2_insn {
	int opcode;
	const char* mnemonic;
	mapip2_operand operands[MAX_SYNTAX_ELEMENTS];
} mapip2_insn;

typedef struct mapip2_parse_node {
	mapip2_operand operand;
	const struct mapip2_parse_node* children;
	int nc_op;	// number of children if children != NULL, opcode otherwise.
} mapip2_parse_node;

typedef struct mapip2_mnemonic {
	const char* mnemonic;
	const mapip2_parse_node* children;
	int nc_op;	// number of children if children != NULL, opcode otherwise.
} mapip2_mnemonic;

extern const mapip2_insn mapip2_insns[];
extern const size_t mapip2_insn_count;

extern const mapip2_mnemonic mapip2_mnemonics[];
extern const size_t mapip2_mnemonic_count;

extern const char* const mapip2_register_names[];
extern const size_t mapip2_register_name_count;

extern const char* const mapip2_float_register_names[];
extern const size_t mapip2_float_register_name_count;
