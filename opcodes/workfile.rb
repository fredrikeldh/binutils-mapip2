#!/usr/bin/ruby

require File.expand_path('../common.rb')

class GenOpcodesTask < FileTask
	def initialize(mode, dst)
		@mode = mode
		@gen = "#{CONFIG_MOSYNC_SOURCE_DIR}/runtimes/cpp/core/gen-opcodes.rb"
		@prerequisites = [FileTask.new(@gen)]
		super(dst)
	end
	def fileExecute
		sh "ruby #{@gen} #{@mode} #{@NAME}"
	end
end

BinutilsLibWork.new do
	@REQUIREMENTS += [
		GenOpcodesTask.new('binutils/desc', 'mapip2-gen-desc.h'),
		GenOpcodesTask.new('ccore', 'mapip2-gen-opcodes.h'),
	]

	@EXTRA_INCLUDES << '..'
	@EXTRA_INCLUDES << '../bfd'
	@SOURCE_FILES = [
		#'cgen-asm.c',
		#'cgen-bitset.c',
		#'cgen-dis.c',
		#'cgen-opc.c',
		'disassemble.c',
		'dis-init.c',
		'dis-buf.c',
		"#{CONFIG_TARGET}-dis.c",
		"#{CONFIG_TARGET}-desc.c",
	]
	#@EXTRA_SOURCETASKS = cgenTasks(self, CONFIG_TARGET)
	@SPECIFIC_CFLAGS = {
		#"#{CONFIG_TARGET}-opc.c" => ' -Wno-old-style-definition',
		'disassemble.c' => " -DARCH_#{CONFIG_TARGET}",
	}
	@NAME = 'opcodes'
end

Works.run
