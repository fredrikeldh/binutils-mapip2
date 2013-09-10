#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/cExe.rb')
require File.expand_path(ENV['MOSYNCDIR']+'/rules/subdir.rb')
require File.expand_path('./config.rb')

intl = []
intl << 'intl' if(HOST != :win32)

MAIN_DIRS = intl + [
'bfd',
'opcodes',
'libiberty',
'binutils',
'gas',
'ld',
#'gdb',
]

target :default do
	Works.invoke_subdirs(MAIN_DIRS)
end

target :run => :default do
	CONFIG_RUN_FUNCTION()
end

Works.run
