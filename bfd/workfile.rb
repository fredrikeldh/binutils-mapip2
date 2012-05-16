#!/usr/bin/ruby

require File.expand_path('../common.rb')

work = BinutilsLibWork.new
work.instance_eval do
	@SOURCES = []
	@EXTRA_SOURCEFILES = [
		"#{CONFIG_TARGET}.c",
		"cpu-#{CONFIG_TARGET}.c",
		'archive.c',
		'archures.c',
		'bfd.c',
		'bfdio.c',
		'bfdwin.c',
		'binary.c',
		'bout.c',
		'cache.c',
		'coffgen.c',
		'compress.c',
		'corefile.c',
		'dwarf1.c',
		'dwarf2.c',
		'ecoff.c',
		'ecofflink.c',
		'elf.c',
		'elf-attrs.c',
		'elf-eh-frame.c',
		'elf-strtab.c',
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
		'srec.c',
		'stabs.c',
		'stab-syms.c',
		'syms.c',
		'targets.c',
		'verilog.c',
		'tekhex.c',
		'ihex.c',
	]
	@SPECIFIC_CFLAGS = {
		'ecofflink.c' => ' -Wno-old-style-definition',
		'archures.c' => " -DSELECT_ARCHITECTURES=&bfd_#{CONFIG_TARGET}_arch",
		'targets.c' => " -DSELECT_VECS=&#{CONFIG_TARGET}_vec",
		'dwarf2.c' => ' -DDEBUGDIR=\"./debug\"'
	}
	@NAME = 'bfd'
end

work.invoke
