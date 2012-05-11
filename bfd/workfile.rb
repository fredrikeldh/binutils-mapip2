#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/native_lib.rb')

#class ConfigHeaderTask < MemoryGeneratedFileTask
#	def initialize
#		io = StringIO.new
#		io.write("\n")
#		io.write("\n")
#		@buf = io.string
#	end
#end

class ConfigHeaderTask < CopyFileTask
	def initialize(work)
		super(work, 'config.h', FileTask.new(work, 'config.h.example'))
	end
end

work = NativeLibWork.new
work.instance_eval do
	@PREREQUISITES = [
		ConfigHeaderTask.new(self)
	]
	@SOURCES = []
	@EXTRA_SOURCEFILES = [
		'mapip2.c',
		'cpu-mapip2.c',
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
	@EXTRA_INCLUDES = ['../include']
	@EXTRA_CFLAGS = ''+
		' -Wno-declaration-after-statement'+
		' -DARCH_SIZE=32'+
		''
	@SPECIFIC_CFLAGS = {
		'ecofflink.c' => ' -Wno-old-style-definition',
	}
	@NAME = 'bfd'
end

work.invoke
