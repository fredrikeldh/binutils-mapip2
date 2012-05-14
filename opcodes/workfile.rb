#!/usr/bin/ruby

require File.expand_path('../common.rb')

class CgenTask < FileTask
	def initialize(work, name, cmd)
		super(work, name)
		@cmd = cmd
	end
	def execute
		sh @cmd
	end
end

work = BinutilsLibWork.new
work.instance_eval do
	def cgenTasks(work, target)
		cmd = "sh -x ./cgen.sh opcodes . \"guile --debug\" #{CONFIG_CGEN_DIR}/cgen \"-v\" #{CONFIG_TARGET} #{CONFIG_TARGET}"+
			" ../cpu/#{CONFIG_TARGET}.cpu ../cpu/#{CONFIG_TARGET}.opc \"\""
		postfixes = [
			'desc.h',
			'desc.c',
			'opc.h',
			'opc.c',
			'opinst.c',
			'ibld.h',
			'ibld.c',
			'asm.c',
			'asm.c',
		]
		# constructing one task per target file ensures rebuilding
		# if any of the files are missing or out-of-date.
		return postfixes.collect {|p| CgenTask.new(self, "#{CONFIG_TARGET}-#{p}", cmd)}
	end

	@SOURCES = []
	@EXTRA_SOURCEFILES = [
		'cgen-asm.c',
		'cgen-bitset.c',
		'cgen-dis.c',
		'cgen-opc.c',
		'disassemble.c',
	]
	@EXTRA_SOURCETASKS = cgenTasks(self, CONFIG_TARGET)
	@SPECIFIC_CFLAGS = {
	}
	@NAME = 'opcodes'
end

work.invoke
