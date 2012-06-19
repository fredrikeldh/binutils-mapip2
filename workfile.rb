#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/exe.rb')
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
]

target :default do
	Work.invoke_subdirs(MAIN_DIRS)
end

target :clean do
	verbose_rm_rf('build')
	Work.invoke_subdirs(MAIN_DIRS, 'clean')
end

target :run => :default do
	CONFIG_RUN_FUNCTION()
end

Targets.invoke
