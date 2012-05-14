#!/usr/bin/ruby

require File.expand_path('../common.rb')

work = BinutilsLibWork.new
work.instance_eval do
	@SOURCES = []
	@EXTRA_SOURCEFILES = [
		"#{CONFIG_TARGET}.c",
		"cpu-#{CONFIG_TARGET}.c",
		'archures.c',
		'bfd.c',
		'bfdio.c',
		'bfdwin.c',
		'binary.c',
		'bout.c',
		'cache.c',
		'compress.c',
		'corefile.c',
		'ecoff.c',
		'ecofflink.c',
		'elf.c',
		'elflink.c',
		'format.c',
		'hash.c',
		'init.c',
		'libbfd.c',
		'linker.c',
		'merge.c',
		'opncls.c',
		'plugin.c',
		'ptrace-core.c',
		'reloc.c',
		'section.c',
		'simple.c',
		'stabs.c',
		'stab-syms.c',
		'syms.c',
		'targets.c',
	]
	@SPECIFIC_CFLAGS = {
		'ecofflink.c' => ' -Wno-old-style-definition',
	}
	@NAME = 'bfd'
end

work.invoke
