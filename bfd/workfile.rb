#!/usr/bin/ruby

require File.expand_path('../common.rb')

class XXTask < FileTask
	def initialize(name, src, xx, replacement)
		@prerequisites = [FileTask.new(src)]
		@src = src
		@xx = xx
		@replacement = replacement
		super(name)
	end
	def fileExecute
		sh "sed -e s/#{@xx}/#{@replacement}/g < #{@src} > #{@NAME}"
	end
end

BinutilsLibWork.new do
	@REQUIREMENTS = [XXTask.new('elf32-target.h', 'elfxx-target.h', 'NN', '32')]
	@SOURCE_FILES = [
		"#{CONFIG_TARGET}.c",
		"cpu-#{CONFIG_TARGET}.c",
		'archive.c',
		'archive64.c',
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
		'elf32.c',
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
		'archures.c' => " \"-DSELECT_ARCHITECTURES=&bfd_#{CONFIG_TARGET}_arch\"",
		'targets.c' => " \"-DSELECT_VECS=&#{CONFIG_TARGET}_vec\" -DHAVE_#{CONFIG_TARGET}_vec",
		'dwarf2.c' => ' -DDEBUGDIR=\"./debug\"',
		'bfd.c' => ' -Wno-missing-format-attribute',
		'archive.c' => ' -Wno-nested-externs',
		'plugin.c' => ' -Wno-missing-format-attribute -DBINDIR=\"\"',
 	}
	@SPECIFIC_CFLAGS['plugin.c'] << ' -Wno-vla' if(@GCC_V4_SUB >= 3)
	@NAME = 'bfd'
end

Works.run
