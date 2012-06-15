#!/usr/bin/ruby

require File.expand_path('../common.rb')

class CgenTask < FileTask
	def initialize(work, name, cmd, prereqs)
		super(work, name)
		@cmd = cmd
		@prerequisites = prereqs
	end
	def execute
		sh @cmd
		sh "sed -e \"/ -- assembler routines/ r #{CONFIG_TARGET}-asm.in1\" cgen-asm.in"+
			" | sed -e \"s/@ARCH@/#{CONFIG_TARGET.upcase}/g\" -e \"s/@arch@/#{CONFIG_TARGET}/g\""+
			" -e \"s/@prefix@/#{CONFIG_TARGET}/\" > #{CONFIG_TARGET}-asm.c"
		sh "sed -e \"/ -- assembler routines/ r #{CONFIG_TARGET}-dis.in1\" cgen-dis.in"+
			" | sed -e \"s/@ARCH@/#{CONFIG_TARGET.upcase}/g\" -e \"s/@arch@/#{CONFIG_TARGET}/g\""+
			" -e \"s/@prefix@/#{CONFIG_TARGET}/\" > #{CONFIG_TARGET}-dis.c"
		sh "rm -f tmp-*"
	end
end

class GenOpcodesTask < FileTask
	def initialize(work)
		super(work, '../cpu/mapip2-opcodes.inc')
		@gen = "#{CONFIG_MOSYNC_SOURCE_DIR}/runtimes/cpp/core/gen-opcodes.rb"
		prerequisites << FileTask.new(work, @gen)
	end
	def execute
		sh "ruby #{@gen} cgen #{@NAME}"
	end
end

work = BinutilsLibWork.new
work.instance_eval do
	def cgenTasks(work, target)
		cpuFile = "../cpu/#{CONFIG_TARGET}.cpu"
		opcFile = "../cpu/#{CONFIG_TARGET}.opc"
		cmd = "bash -x ./cgen.sh opcodes . \"guile --debug -l #{CONFIG_CGEN_DIR}/cgen/guile.scm"+
			" -s\" #{CONFIG_CGEN_DIR}/cgen \"-v\" #{CONFIG_TARGET} #{CONFIG_TARGET}"+
			" #{cpuFile} #{opcFile} \"\""
		postfixes = [
			'desc.h',
			'desc.c',
			'opc.h',
			'opc.c',
			#'opinst.c',
			'ibld.h',
			'ibld.c',
			'asm.c',
			'asm.c',
		]
		# constructing one task per target file ensures rebuilding
		# if any of the files are missing or out-of-date.
		return postfixes.collect {|p| CgenTask.new(self, "#{CONFIG_TARGET}-#{p}", cmd, [
			FileTask.new(self, cpuFile),
			FileTask.new(self, opcFile),
			])}
	end

	@PREREQUISITES = [
		GenOpcodesTask.new(self)
	]

	@SOURCES = []
	@EXTRA_INCLUDES << '..'
	@EXTRA_INCLUDES << '../bfd'
	@EXTRA_SOURCEFILES = [
		'cgen-asm.c',
		'cgen-bitset.c',
		'cgen-dis.c',
		'cgen-opc.c',
		'disassemble.c',
	]
	@EXTRA_SOURCETASKS = cgenTasks(self, CONFIG_TARGET)
	@SPECIFIC_CFLAGS = {
		"#{CONFIG_TARGET}-opc.c" => ' -Wno-old-style-definition',
	}
	@NAME = 'opcodes'
end

work.invoke
