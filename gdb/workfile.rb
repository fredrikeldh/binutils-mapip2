#!/usr/bin/ruby

require File.expand_path('../common.rb')

class SimpleGenFileTask < FileTask
	def initialize(work, name, cmd, prereqs = [])
		super(work, name)
		@cmd = cmd
		prereqs.each do |p|
			@prerequisites << FileTask.new(work, p)
		end
	end
	def execute
		sh @cmd
	end
end

work = BinutilsExeWork.new
work.instance_eval do
	@PREREQUISITES = [
		SimpleGenFileTask.new(self, 'observer.h', './observer.sh h doc/observer.texi observer.h',
			['observer.sh', 'doc/observer.texi']),
		SimpleGenFileTask.new(self, 'observer.inc', './observer.sh inc doc/observer.texi observer.inc',
			['observer.sh', 'doc/observer.texi']),
		#SimpleGenFileTask.new(self, 'jit-reader.h', './config.status jit-reader.in',
			#['config.status', 'jit-reader.in']),
	]
	@SOURCES = ['cli', 'mi']
	@EXTRA_SOURCEFILES = [
		'utils.c',
	]
	@EXTRA_SOURCETASKS = [
		#YaccTask.new(self, 'deffilep.y'),
		#YaccTask.new(self, 'ldgram.y'),
	]
	@EXTRA_INCLUDES << '.'
	@EXTRA_INCLUDES << 'common'
	@EXTRA_INCLUDES << '../bfd'
	@EXTRA_INCLUDES << '../opcodes'
	if(HOST == :win32)
		@EXTRA_SOURCEFILES += Dir['windows-*.c']
		@EXTRA_SOURCEFILES += Dir['*-windows-*.c']
	elsif(HOST == :linux)
		#@EXTRA_SOURCEFILES += Dir['linux-*.c']
		#@EXTRA_SOURCEFILES += Dir['*-linux.c']
	end
	@IGNORED_FILES = [
		#'linux-thread-db.c',
	]
	@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1 -DLOCALEDIR=0'
	@EXTRA_CFLAGS << ' -Wno-cast-qual'
	@EXTRA_CFLAGS << ' -Wno-sign-compare -Wno-shadow -Wno-unused-variable -Wno-switch'
	@EXTRA_CFLAGS << ' -Wno-missing-field-initializers -Wno-nested-externs'
	@EXTRA_CFLAGS << ' -Wno-missing-prototypes -Wno-missing-declarations'
	@EXTRA_CFLAGS << ' -Wno-pointer-sign'
	@SPECIFIC_CFLAGS = {
		'varobj.c' => ' -DHAVE_PYTHON=0',
		'linux-thread-db.c' => ' -DHAVE_THREAD_DB_H=1 -include config/nm-linux.h',
		'cli-cmds.c' => ' -DHAVE_VFORK_H=0',
		'completer.c' => ' -DHAVE_DOS_BASED_FILE_SYSTEM=0',
		'f-exp.c' => ' -Wno-undef -Wno-old-style-definition',
		'jv-exp.c' => ' -Wno-undef -Wno-old-style-definition',
		'dwarf2loc.c' => ' -fsigned-char',
		'dwarf2read.c' => ' -DWORDS_BIGENDIAN=0 -Wno-pointer-sign',
		'event-top.c' => ' -DHAVE_SIGPROCMASK=0 -DHAVE_SIGSETMASK=0',
		'valarith.c' => ' -Wno-float-equal',
		'top.c' => ' -DPKGVERSION=\"mapip2\" -DREPORT_BUGS_TO=\"bugs@mosync.com\"',
		'disasm.c' => ' -Wno-missing-format-attribute',
		'linux-nat.c' => ' -DPTRACE_TYPE_ARG3=long',
		'main.c' => ' -DBINDIR=0 -DSYSTEM_GDBINIT=\"\" -DSYSTEM_GDBINIT_RELOCATABLE=0'+
			' -DTARGET_SYSTEM_ROOT=\"\" -DTARGET_SYSTEM_ROOT_RELOCATABLE=0'+
			' -DDEBUGDIR=\"\" -DDEBUGDIR_RELOCATABLE=0'+
			' -DGDB_DATADIR=\"\" -DGDB_DATADIR_RELOCATABLE=0'+
			' -DREPORT_BUGS_TO=\"bugs@mosync.com\"',
		'mi-main.c' => ' -DHAVE_PYTHON=0',
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
	@NAME = 'gdb'
end

#work.invoke

work = Work.new
work.instance_eval do
	@prerequisites = [
		SimpleGenFileTask.new(self, '../build/release/gdb/Makefile', './configure-linux-release.sh',
			['configure-linux-release.sh']),
		#CopyFileTask.new(self, '../build/release/bfd/libbfd.a',
	]
	def setup
	end
end

work.invoke

sh 'cd ../build/release/libdecnumber && make'
sh 'cd ../build/release/gdb && make'
