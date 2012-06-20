#!/usr/bin/ruby

require File.expand_path('../common.rb')

work = BinutilsExeWork.new
work.instance_eval do
	@SOURCES = ['.']
	@EXTRA_SOURCEFILES = [
		"config/tc-#{CONFIG_TARGET}.c",
		'config/atof-ieee.c',
		'config/obj-elf.c',
	]
	@EXTRA_INCLUDES << 'config'
	@EXTRA_INCLUDES << '.'
	@EXTRA_INCLUDES << '../bfd'
	@EXTRA_INCLUDES << '../opcodes'
	@IGNORED_FILES = [
		'debug.c',
		'itbl-ops.c',
		'cgen.c',
	]
	@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1 -DLOCALEDIR'
	@SPECIFIC_CFLAGS = {
		'cgen.c' => ' -Wno-old-style-definition',
		#'itbl-ops.c' => ' -Wno-nested-externs',
		'listing.c' => ' -Wno-missing-format-attribute',
	}
	@LOCAL_LIBS = [
		'bfd',
		'opcodes',
		'libiberty',
	]
	@LIBRARIES = [
		'z',
	]
	if(HOST == :win32)
		@LIBRARIES << 'intl'
	else
		@LOCAL_LIBS << 'intl'
	end
	@NAME = 'as'
end

work.invoke
