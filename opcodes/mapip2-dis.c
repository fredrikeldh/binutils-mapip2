
#include "sysdep.h"
#include "dis-asm.h"
#include "mapip2-desc.h"

static const mapip2_insn* get_mapip2_opcode_info(bfd_byte opcode) {
	if(opcode < mapip2_insn_count)
		return &mapip2_insns[opcode];
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

#if 1
static int ri(bfd_vma addr, disassemble_info* info) {
	int i;
	int status = info->read_memory_func(addr, (bfd_byte*)&i, 4, info);

	if (status == 0)
		return i;

	info->memory_error_func(status, addr, info);
	return -1;
}
#endif

static void doRegister(mapip2_operand op, bfd_vma addr, disassemble_info* info) {
	void *stream = info->stream;
	fprintf_ftype pf = info->fprintf_func;
	bfd_byte r = rb(addr, info);
	if(r < mapip2_register_name_count) {
		pf(stream, "%s", mapip2_register_names[r]);
	} else {
		pf(stream, "%s(invalid %i)", op == RD ? "rd" : "rs", r);
	}
}

static void doImm(bfd_vma addr, disassemble_info* info) {
#if 1
	void *stream = info->stream;
	fprintf_ftype pf = info->fprintf_func;
	int word = ri(addr, info);
	addr += 4;
	pf(stream, "0x%x", word);
#endif
	//info->print_address_func(word, info);
}

// returns the number of bytes read.
int print_insn_mapip2(bfd_vma addr, disassemble_info* info) {
	bfd_vma startAddr = addr;
	void *stream = info->stream;
	fprintf_ftype pf = info->fprintf_func;
	bfd_byte opcode = rb(addr, info);
	addr++;
	const mapip2_insn* insn = get_mapip2_opcode_info(opcode);
	if(!insn) {
		pf(stream, "????", opcode);
		return 1;
	}
	pf(stream, "%s", insn->mnemonic);
	for(int i=0; i<MAX_SYNTAX_ELEMENTS; i++) {
		mapip2_operand op = insn->operands[i];
		if(op == END)
			break;
		pf(stream, (i == 0) ? " " : ",");
		switch(op) {
		case RD:
		case RS:
			doRegister(op, addr++, info);
			break;
		case IMM8:
			pf(stream, "%i", rb(addr++, info));
			break;
		case IMM:
		case AIADDR:
		case RIADDR:	// relative jumps not yet implemented; handled as absolute for now.
			doImm(addr, info);
			addr += 4;
			break;
		case ADADDR:
			pf(stream, "[");
			doRegister(op, addr++, info);
			pf(stream, ",");
			if(addr - startAddr == 2) {	// Store
				doImm(addr + 1, info);
				pf(stream, "],");
				// do the source register, too.
				doRegister(RS, addr++, info);
				i++;
			} else {
				doImm(addr, info);
				pf(stream, "]");
			}
			addr += 4;
			break;
		case END:
			abort();
		}
	}
	return addr - startAddr;
}
