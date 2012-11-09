
require "#{File.dirname(__FILE__)}/config.rb"

require File.expand_path(ENV['MOSYNCDIR']+'/rules/native_lib.rb')
require File.expand_path(ENV['MOSYNCDIR']+'/rules/exe.rb')

class GenTask < FileTask
	def initialize(work, src)
		@src = src
		@dst = src.ext('.c')
		super(work, @dst)
		@prerequisites << FileTask.new(work, @src)
	end
end

class YaccTask < GenTask
	def execute
		sh "bison -y -o #{@dst} #{@src}"
	end
end

class FlexTask < GenTask
	def execute
		sh "flex -o #{@dst} #{@src}"
	end
end

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
		if(HOST == :darwin)
			@EXTRA_INCLUDES << '/opt/local/include'
			@EXTRA_CFLAGS << ' -Wno-unreachable-code'
		end
	end
end

class BinutilsLibWork < NativeLibWork
	include BinutilsModule
end

class BinutilsExeWork < ExeWork
	include BinutilsModule
	def initialize
		super
		@INSTALLDIR = CONFIG_INSTALLDIR if(CONFIG_INSTALLDIR)
	end
end
