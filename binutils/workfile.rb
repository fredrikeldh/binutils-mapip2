#!/usr/bin/ruby

require File.expand_path('../common.rb')

class BBEW < BinutilsExeWork
	def initialize(name, sourcefiles)
		@NAME = name
		@SOURCE_FILES = sourcefiles
		@EXTRA_INCLUDES = ['.', '../bfd', '../opcodes']
		@EXTRA_CFLAGS = ' -DHAVE_CONFIG_H=1 -DLOCALEDIR=\"\" -DUSING_CGEN=1 -DHAVE_DECL_FPRINTF=1'
		@EXTRA_CFLAGS << ' -Wno-missing-format-attribute'
		@SPECIFIC_CFLAGS = {
			'rename.c' => ' -DHAVE_GOOD_UTIME_H=1',
			'objdump.c' => ' -DOBJDUMP_PRIVATE_VECTORS=""',
			'cxxfilt.c' => ' -DTARGET_PREPENDS_UNDERSCORE=1',
			'bucomm.c' => " -DTARGET=\\\"#{CONFIG_TARGET}-unknown-elf\\\"",
			'arsup.c' => ' -Wno-nested-externs',
			'binemul.c' => ' -Dbin_dummy_emulation=bin_vanilla_emulation',
			'arlex.c' => ' -Wno-sign-compare',
		}
		@LOCAL_LIBS = [
			'bfd',
			'opcodes',
			'libiberty',
		]
		@LIBRARIES = [
			'z',
			'dl',
		]
		if(HOST == :win32)
			@LIBRARIES << 'intl'
		else
			@LOCAL_LIBS << 'intl'
		end
		super()
	end
end

# common sources
DEBUG_SRCS = ['rddbg.c','debug.c','stabs.c','ieee.c','rdcoff.c']
WRITE_DEBUG_SRCS = DEBUG_SRCS + ['wrstabs.c']

BULIBS = ['bucomm.c','version.c','filemode.c']

ELFLIBS = ['elfcomm.c']

class ArParseTask < YaccTask
	def fileExecute
		sh "bison -y -o #{@dst} #{@src} --defines"
	end
end

ar = BBEW.new('ar', BULIBS +
	[
		'ar.c',
		'arsup.c',
		'binemul.c',
		'emul_vanilla.c',
		'rename.c',
		'maybe-ranlib.c',
	]) do
	y = ArParseTask.new('arparse.y')
	f = FlexTask.new('arlex.l', [y])
	@SOURCE_TASKS = [
		y,
		f,
	]
end

BBEW.new('objdump', ['objdump.c','dwarf.c','prdbg.c'] + DEBUG_SRCS + BULIBS + ELFLIBS)
BBEW.new('size', ['size.c'] + BULIBS)
BBEW.new('objcopy', ['objcopy.c','not-strip.c','rename.c'] + WRITE_DEBUG_SRCS + BULIBS)
BBEW.new('strings', ['strings.c'] + BULIBS)
BBEW.new('readelf', ['readelf.c','version.c','unwind-ia64.c','dwarf.c'] + ELFLIBS)
BBEW.new('elfedit', ['elfedit.c','version.c'] + ELFLIBS)
BBEW.new('strip_new', ['objcopy.c','is-strip.c','rename.c'] + WRITE_DEBUG_SRCS + BULIBS)
BBEW.new('nm_new', ['nm.c'] + BULIBS)
BBEW.new('cxxfilt', ['cxxfilt.c'] + BULIBS)
BBEW.new('addr2line', ['addr2line.c'] + BULIBS)

Works.run
