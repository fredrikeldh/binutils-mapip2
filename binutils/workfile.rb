#!/usr/bin/ruby

require File.expand_path('../common.rb')

class BBEW < BinutilsExeWork
	def initialize(name, sourcefiles)
		super()
		@NAME = name
		@EXTRA_SOURCEFILES = sourcefiles
		@SOURCES = []
		@EXTRA_INCLUDES << '.'
		@EXTRA_INCLUDES << '../bfd'
		@EXTRA_INCLUDES << '../opcodes'
		@IGNORED_FILES = [
		]
		@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1 -DLOCALEDIR=\"\" -DUSING_CGEN=1 -DHAVE_DECL_FPRINTF=1'
		@EXTRA_CFLAGS << ' -Wno-missing-format-attribute'
		@SPECIFIC_CFLAGS = {
			'rename.c' => ' -DHAVE_GOOD_UTIME_H=1',
			'objdump.c' => ' -DOBJDUMP_PRIVATE_VECTORS=""',
			'cxxfilt.c' => ' -DTARGET_PREPENDS_UNDERSCORE=1',
			'bucomm.c' => " -DTARGET=\\\"#{CONFIG_TARGET}-unknown-elf\\\""
		}
		@LOCAL_LIBS = [
			'bfd',
			'opcodes',
			'libiberty',
		]
		@LIBRARIES = [
			'z',
		]
		if(HOST == :win32)
			@LIBRARIES << 'intl'
		else
			@LOCAL_LIBS << 'intl'
		end
	end
end

# common sources
DEBUG_SRCS = ['rddbg.c','debug.c','stabs.c','ieee.c','rdcoff.c']
WRITE_DEBUG_SRCS = DEBUG_SRCS + ['wrstabs.c']

BULIBS = ['bucomm.c','version.c','filemode.c']

ELFLIBS = ['elfcomm.c']


works = [
	BBEW.new('objdump', ['objdump.c','dwarf.c','prdbg.c'] + DEBUG_SRCS + BULIBS + ELFLIBS),
	BBEW.new('size', ['size.c'] + BULIBS),
	BBEW.new('objcopy', ['objcopy.c','not-strip.c','rename.c'] + WRITE_DEBUG_SRCS + BULIBS),
	BBEW.new('strings', ['strings.c'] + BULIBS),
	BBEW.new('readelf', ['readelf.c','version.c','unwind-ia64.c','dwarf.c'] + ELFLIBS),
	BBEW.new('elfedit', ['elfedit.c','version.c'] + ELFLIBS),
	BBEW.new('strip_new', ['objcopy.c','is-strip.c','rename.c'] + WRITE_DEBUG_SRCS + BULIBS),
	BBEW.new('nm_new', ['nm.c'] + BULIBS),
	BBEW.new('cxxfilt', ['cxxfilt.c'] + BULIBS),
	BBEW.new('addr2line', ['addr2line.c'] + BULIBS),
]

works.each do |w| w.invoke end
