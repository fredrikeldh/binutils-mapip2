
#include "sysdep.h"
#include "dis-asm.h"

typedef struct mapip2_opcode_info mapip2_opcode_info;

static const mapip2_opcode_info* get_mapip2_opcode_info(bfd_byte opcode) {
	return NULL;
}

static bfd_byte rb(bfd_vma addr, disassemble_info* info) {
	bfd_byte b;
	int status = info->read_memory_func(addr, &b, 1, info);

	if (status == 0)
		return b;

	info->memory_error_func(status, addr, info);
	return -1;
}

// returns the number of bytes read.
int print_insn_mapip2(bfd_vma addr, disassemble_info* info) {
	void *stream = info->stream;
	fprintf_ftype pf = info->fprintf_func;
	bfd_byte opcode = rb(addr, info);
	const mapip2_opcode_info* oi = get_mapip2_opcode_info(opcode);
	if(!oi) {
		pf(stream, "0x%02x\t//????\n", opcode);
		return 1;
	}
	// todo
	abort();
}
