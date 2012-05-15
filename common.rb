
require "#{File.dirname(__FILE__)}/config.rb"

require File.expand_path(ENV['MOSYNCDIR']+'/rules/native_lib.rb')
require File.expand_path(ENV['MOSYNCDIR']+'/rules/exe.rb')

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
		super(work, '../config.h', FileTask.new(work, '../config.h.example'))
	end
end

module BinutilsModule
	def initialize
		super
		@PREREQUISITES = [
			ConfigHeaderTask.new(self)
		]
		@EXTRA_INCLUDES = ['../include', '..']
		@EXTRA_CFLAGS = ''+
			' -Wno-declaration-after-statement'+
			' -DARCH_SIZE=32'+
			''
	end
end

class BinutilsLibWork < NativeLibWork
	include BinutilsModule
end

class BinutilsExeWork < ExeWork
	include BinutilsModule
end
