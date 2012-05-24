#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/exe.rb')
require './config.rb'

MAIN_DIRS = [
'bfd',
'opcodes',
'libiberty',
#'intl',
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
