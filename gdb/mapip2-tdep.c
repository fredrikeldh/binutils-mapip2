#include "defs.h"
#include "osabi.h"
#include "gdbtypes.h"
#include "dis-asm.h"
#include "arch-utils.h"
#include "frame.h"
#include "../opcodes/mapip2-desc.h"
#include "../opcodes/mapip2-gen-opcodes.h"

static void mapip2_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
	fprintf(stderr, "mapip2_init_abi\n");
}

#define bfd_mach_mapip2 0	// matches bfd/cpu-mapip2.c

/* Return the name of the REGNO register.

   An empty name corresponds to a register number that used to
   be used for a virtual register.  That virtual register has
   been removed, but the index is still reserved to maintain
   compatibility with existing remote alpha targets.  */

static const char *
mapip2_register_name (struct gdbarch *gdbarch, int regno)
{
	if (regno < 0)
		return NULL;
	if (regno >= mapip2_register_name_count)
		return NULL;
	return mapip2_register_names[regno];
}

static int
mapip2_cannot_store_register (struct gdbarch *gdbarch, int regno)
{
	return regno == REG_zr;
}

static struct type *
mapip2_register_type (struct gdbarch *gdbarch, int regno)
{
	//if (regno == ALPHA_SP_REGNUM || regno == ALPHA_GP_REGNUM)
	return builtin_type (gdbarch)->builtin_data_ptr;
#if 0
	if (regno == ALPHA_PC_REGNUM)
		return builtin_type (gdbarch)->builtin_func_ptr;

	/* Don't need to worry about little vs big endian until
	some jerk tries to port to alpha-unicosmk.  */
	if (regno >= ALPHA_FP0_REGNUM && regno < ALPHA_FP0_REGNUM + 31)
		return builtin_type (gdbarch)->builtin_double;

	return builtin_type (gdbarch)->builtin_int64;
#endif
}

static const gdb_byte *
mapip2_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pc, int *len)
{
	static const gdb_byte break_insn[] = { OP_BREAK };
	*len = sizeof(break_insn);
	return break_insn;
}

static CORE_ADDR
mapip2_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
	// todo
	return pc;
}

static int MAPIP2_PC_REGNUM = 32;
static CORE_ADDR mapip2_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, MAPIP2_PC_REGNUM);
}


/* Initialize the current architecture based on INFO.  If possible, re-use an
   architecture from ARCHES, which is a list of architectures already created
   during this debugging session.

   Called e.g. at program startup, when reading a core file, and when reading
   a binary file.  */

static struct gdbarch *
mapip2_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
	struct gdbarch *gdbarch;

	fprintf(stderr, "mapip2_gdbarch_init\n");

	/* Find a candidate among extant architectures.  */
	arches = gdbarch_list_lookup_by_info (arches, &info);
	if (arches != NULL)
		return arches->gdbarch;

	gdbarch = gdbarch_alloc (&info, NULL);

	/* Type sizes */
	set_gdbarch_short_bit (gdbarch, 16);
	set_gdbarch_int_bit (gdbarch, 32);
	set_gdbarch_long_bit (gdbarch, 32);
	set_gdbarch_long_long_bit (gdbarch, 64);
	set_gdbarch_float_bit (gdbarch, 32);
	set_gdbarch_double_bit (gdbarch, 64);
	set_gdbarch_long_double_bit (gdbarch, 64);
	set_gdbarch_ptr_bit (gdbarch, 32);

	/* Register info */
	set_gdbarch_num_regs (gdbarch, 33);
	set_gdbarch_sp_regnum (gdbarch, 1);
	set_gdbarch_pc_regnum (gdbarch, MAPIP2_PC_REGNUM);
	//set_gdbarch_fp0_regnum (gdbarch, ALPHA_FP0_REGNUM);

	set_gdbarch_register_name (gdbarch, mapip2_register_name);
	set_gdbarch_register_type (gdbarch, mapip2_register_type);

	//set_gdbarch_cannot_fetch_register (gdbarch, alpha_cannot_fetch_register);
	set_gdbarch_cannot_store_register (gdbarch, mapip2_cannot_store_register);

	//set_gdbarch_convert_register_p (gdbarch, alpha_convert_register_p);
	//set_gdbarch_register_to_value (gdbarch, alpha_register_to_value);
	//set_gdbarch_value_to_register (gdbarch, alpha_value_to_register);

	//set_gdbarch_register_reggroup_p (gdbarch, alpha_register_reggroup_p);

	/* Prologue heuristics.  */
	set_gdbarch_skip_prologue (gdbarch, mapip2_skip_prologue);

	/* Disassembler.  */
	set_gdbarch_print_insn (gdbarch, print_insn_mapip2);

	/* Call info.  */

	//set_gdbarch_return_value (gdbarch, mapip2_return_value);

	/* Settings for calling functions in the inferior.  */
	//set_gdbarch_push_dummy_call (gdbarch, alpha_push_dummy_call);

	/* Methods for saving / extracting a dummy frame's ID.  */
	//set_gdbarch_dummy_id (gdbarch, alpha_dummy_id);

	/* Return the unwound PC value.  */
	set_gdbarch_unwind_pc (gdbarch, mapip2_unwind_pc);

	set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
	//set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

	set_gdbarch_breakpoint_from_pc (gdbarch, mapip2_breakpoint_from_pc);
	//set_gdbarch_decr_pc_after_break (gdbarch, ALPHA_INSN_SIZE);
	set_gdbarch_cannot_step_breakpoint (gdbarch, 1);

	/* Handles single stepping of atomic sequences.  */
	//set_gdbarch_software_single_step (gdbarch, alpha_deal_with_atomic_sequence);

	/* Hook in ABI-specific overrides, if they have been registered.  */
	gdbarch_init_osabi (info, gdbarch);

	/* Now that we have tuned the configuration, set a few final things
	based on what the OS ABI has told us.  */

	//frame_unwind_append_unwinder (gdbarch, &alpha_sigtramp_frame_unwind);
	//frame_unwind_append_unwinder (gdbarch, &alpha_heuristic_frame_unwind);

	//frame_base_set_default (gdbarch, &alpha_heuristic_frame_base);

	return gdbarch;
}


void
_initialize_mapip2_tdep (void)
{
	fprintf(stderr, "_initialize_mapip2_tdep\n");
	gdbarch_register(bfd_arch_mapip2, mapip2_gdbarch_init, NULL);
	gdbarch_register_osabi(bfd_arch_mapip2, bfd_mach_mapip2,
		GDB_OSABI_MOSYNC, mapip2_init_abi);
}
