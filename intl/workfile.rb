#!/usr/bin/ruby

require File.expand_path('../common.rb')

work = BinutilsLibWork.new
work.instance_eval do
	@SOURCES = ['.']
	@IGNORED_FILES = [
	]
	@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1'+
		' -Wno-undef -Wno-old-style-definition -Wno-shadow -Wno-strict-prototypes -Wno-nested-externs'+
		' -DLOCALEDIR=\".\"'+
		' -DIN_LIBINTL=1'
	@SPECIFIC_CFLAGS = {
		'loadmsgcat.c' => ' -Wno-sign-compare',
		'intl-compat.c' => ' -Wno-missing-declarations -Wno-missing-prototypes',
		'log.c' => ' -Wno-missing-declarations -Wno-missing-prototypes',
		'textdomain.c' => ' -Wno-missing-declarations -Wno-missing-prototypes',
		'localealias.c' => ' -DLOCALE_ALIAS_PATH=\".\"',
	}
	@NAME = 'intl'
end

work.invoke
