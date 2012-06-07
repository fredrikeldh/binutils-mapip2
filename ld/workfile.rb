#!/usr/bin/ruby

require File.expand_path('../common.rb')

work = BinutilsExeWork.new
work.instance_eval do
	@SOURCES = ['.']
	@EXTRA_SOURCEFILES = [
	]
	@EXTRA_SOURCETASKS = [
		YaccTask.new(self, 'deffilep.y'),
		YaccTask.new(self, 'ldgram.y'),
	]
	@PREREQUISITES = [
		FlexTask.new(self, 'ldlex.l'),
	]
	@EXTRA_INCLUDES << '.'
	@EXTRA_INCLUDES << '../bfd'
	@IGNORED_FILES = [
		'plugin.c', # due to lack of dlopen()
		'ldlex.c',	# wrapped
		'pe-dll.c',	# don't need PE
		'pep-dll.c',	# don't need PE
		'deffilep.c',	# sourcetask
		'ldgram.c',	# sourcetask
	]
	@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1 -DLOCALEDIR=0 -DUSING_CGEN=1'
	@EXTRA_CFLAGS << " -DTARGET=\\\"#{CONFIG_TARGET}\\\""
	@EXTRA_CFLAGS << ' -DYYDEBUG=0'
	@EXTRA_CFLAGS << ' -DBINDIR=0 -DTOOLBINDIR=0'
	@EXTRA_CFLAGS << ' -DSCRIPTDIR=0'
	@EXTRA_CFLAGS << ' -DDEFAULT_EMULATION=0'
	@EXTRA_CFLAGS << ' -DTESTING=0'
	@EXTRA_CFLAGS << ' -Wno-nested-externs'
	@SPECIFIC_CFLAGS = {
		'ldlex-wrapper.c' => ' -Wno-shadow',
	}
	@LOCAL_LIBS = [
		'bfd',
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
	@NAME = 'ld'
end

work.invoke
