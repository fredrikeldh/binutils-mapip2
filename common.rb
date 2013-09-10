
require "#{File.dirname(__FILE__)}/config.rb"

require File.expand_path(ENV['MOSYNCDIR']+'/rules/cLib.rb')
require File.expand_path(ENV['MOSYNCDIR']+'/rules/cExe.rb')

class GenTask < FileTask
	def initialize(src, extraPrereqs = [])
		@src = src
		@dst = src.ext('.c')
		@prerequisites = [FileTask.new(@src)] + extraPrereqs
		super(@dst)
	end
end

class YaccTask < GenTask
	def fileExecute
		sh "bison -y -o #{@dst} #{@src}"
	end
end

class FlexTask < GenTask
	def fileExecute
		sh "flex -o #{@dst} #{@src}"
	end
end

class ConfigHeaderTask < CopyFileTask
	def initialize()
		super('../config.h', FileTask.new('../config.h.example'))
	end
end

module BinutilsModule
	def initialize(compilerModule = DefaultCCompilerModule, &block)
		@compilerModule = compilerModule
		extend compilerModule
		setCompilerVersion

		@REQUIREMENTS ||= []
		@REQUIREMENTS << ConfigHeaderTask.new()
		@EXTRA_INCLUDES ||= []
		@EXTRA_INCLUDES += ['../include', '..']
		@EXTRA_CFLAGS ||= ''
		@EXTRA_CFLAGS << ' -Wno-declaration-after-statement'
		@EXTRA_CFLAGS << ' -DARCH_SIZE=32'
		@EXTRA_CFLAGS << ' -Wno-c++-compat' if(@GCC_V4_SUB >= 5)
		if(HOST == :darwin)
			@EXTRA_INCLUDES << '/opt/local/include'
			@EXTRA_CFLAGS << ' -Wno-unreachable-code'
		end
		super
	end
end

class BinutilsLibWork < LibWork
	include BinutilsModule
end

class BinutilsExeWork < ExeWork
	include BinutilsModule
	def initialize
		@INSTALLDIR = CONFIG_INSTALLDIR if(CONFIG_INSTALLDIR)
		super
	end
end
