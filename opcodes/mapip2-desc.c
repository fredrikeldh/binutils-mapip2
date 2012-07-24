#include <stdint.h>
#include "mapip2-desc.h"
#include "libiberty.h"
#include "mapip2-gen-opcodes.h"

#if 0	// sketch
static const mapip2_parse_node add_rd_[] = {
	{ RS, NULL, OP_ADD },
	{ IMM, NULL, OP_ADDI },
};

static const mapip2_parse_node add_[] = {
	{ RD, add_rd_, ARRAYSIZE(add_rd_) },
};

static const mapip2_mnemonic add = {
	"add", add_, ARRAYSIZE(add_)
};

static const mapip2_mnemonic ret = {
	"ret", NULL, OP_RET
};
#endif

#define _MAPIP2_DESC_H
#include "mapip2-gen-desc.h"

const size_t mapip2_insn_count = ARRAY_SIZE(mapip2_insns);

const size_t mapip2_mnemonic_count = ARRAY_SIZE(mapip2_mnemonics);

const size_t mapip2_register_name_count = ARRAY_SIZE(mapip2_register_names);

const size_t mapip2_float_register_name_count = ARRAY_SIZE(mapip2_float_register_names);
