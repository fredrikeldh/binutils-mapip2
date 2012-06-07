#!/usr/bin/ruby

require File.expand_path('../common.rb')

class XXTask < FileTask
	def initialize(work, name, src, xx, replacement)
		super(work, name)
		@prerequisites = [FileTask.new(work, src)]
		@src = src
		@xx = xx
		@replacement = replacement
	end
	def execute
		sh "sed -e s/#{@xx}/#{@replacement}/g < #{@src} > #{@NAME}"
	end
end

work = BinutilsLibWork.new
work.instance_eval do
	@SOURCES = []
	@PREREQUISITES = [
		XXTask.new(self, 'elf32-target.h', 'elfxx-target.h', 'NN', '32'),
	]
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
		'targets.c' => " \"-DSELECT_VECS=&#{CONFIG_TARGET}_vec\"",
		'dwarf2.c' => ' -DDEBUGDIR=\"./debug\"',
		'bfd.c' => ' -Wno-missing-format-attribute',
	}
	@NAME = 'bfd'
end

work.invoke
