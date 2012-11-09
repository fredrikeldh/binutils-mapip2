#!/usr/bin/ruby

require File.expand_path('../common.rb')

work = BinutilsLibWork.new
work.instance_eval do
	@SOURCES = ['.']
	@IGNORED_FILES = [
		'msdos.c',
		'pex-msdos.c',
		'pex-djgpp.c',
		'calloc.c',
		'strdup.c',
		'strndup.c',
		'strncpy.c',
		'stpncpy.c',
		#'stpcpy.c',
		'clock.c',
		'copysign.c',
		'strncasecmp.c',
		'strcasecmp.c',
		'strverscmp.c',
		'strncmp.c',
		'strchr.c',
		'memchr.c',
		'memcmp.c',
		'memcpy.c',
		'mempcpy.c',
		'memmem.c',
		'memmove.c',
		'memset.c',
		'strrchr.c',
		'strstr.c',
		'bsearch.c',
		'bzero.c',
		'bcopy.c',
		'bcmp.c',
		'ffs.c',
		'getcwd.c',
		'getopt.c',
		'getopt1.c',
		'getpagesize.c',
		'strsignal.c',
		'gettimeofday.c',
		'index.c',
		'rindex.c',
		'insque.c',
		#'make-temp-file.c',
		'strtod.c',
		'strtol.c',
		'strtoul.c',
		'putenv.c',
		'setenv.c',
		'random.c',
		#'mkstemps.c',
		'rename.c',
		'snprintf.c',
		'vfprintf.c',
		'vsprintf.c',
		'vsnprintf.c',
		'vfork.c',
		'waitpid.c',
		'_doprnt.c',
	]
	if(HOST == :win32)
		@IGNORED_FILES << 'pex-unix.c'
	else
		@IGNORED_FILES << 'pex-win32.c'
	end
	@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1'
	@SPECIFIC_CFLAGS = {
		'simple-object-mach-o.c' => ' -Wno-shadow',
		'simple-object-coff.c' => ' -Wno-shadow',
		'simple-object-elf.c' => ' -Wno-shadow',
		'cp-demangle.c' => ' -Wno-shadow',
		'floatformat.c' => ' -Wno-shadow -Wno-float-equal',
		'lrealpath.c' => ' -Wno-shadow',
		'md5.c' => ' -Wno-strict-aliasing',
		'mkstemps.c' => ' -Wno-missing-prototypes -Wno-missing-declarations',
		'physmem.c' => ' -Wno-undef',
		'obstack.c' => ' -Wno-undef',
		'regex.c' => ' -DHAVE_BTOWC=1 -DHAVE_UINTPTR_T=1'+
			' -Wno-missing-prototypes -Wno-missing-declarations -Wno-undef -Wno-shadow',
		'stpcpy.c' => ' -Wno-missing-prototypes -Wno-missing-declarations',
		'vasprintf.c' => ' -Wno-missing-format-attribute',
	}
	def setup
		set_defaults
		@SPECIFIC_CFLAGS['cp-demangle.c'] << ' -Wno-vla' if(@GCC_V4_SUB >= 3)
		@SPECIFIC_CFLAGS['regex.c'] << ' -Wno-type-limits' if(@GCC_V4_SUB >= 3)
		super
	end
	@NAME = 'libiberty'
end

work.invoke
