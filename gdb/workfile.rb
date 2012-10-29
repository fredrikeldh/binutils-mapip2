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
	@PREREQUISITES += [
		SimpleGenFileTask.new(self, 'observer.h', './observer.sh h doc/observer.texi observer.h',
			['observer.sh', 'doc/observer.texi']),
		SimpleGenFileTask.new(self, 'observer.inc', './observer.sh inc doc/observer.texi observer.inc',
			['observer.sh', 'doc/observer.texi']),
		#SimpleGenFileTask.new(self, 'jit-reader.h', './config.status jit-reader.in',
			#['config.status', 'jit-reader.in']),
	]
	@SOURCES = ['cli', 'mi']
	@EXTRA_SOURCEFILES = [
		'ada-exp.c',
		'ada-lang.c',
		'ada-typeprint.c',
		'ada-valprint.c',
		'addrmap.c',
		'annotate.c',
		'arch-utils.c',
		'bcache.c',
		'block.c',
		'blockframe.c',
		'breakpoint.c',
		'buildsym.c',
		'c-exp.c',
		'c-lang.c',
		'c-typeprint.c',
		'c-valprint.c',
		'charset.c',
		'cli-out.c',
		'completer.c',
		'complaints.c',
		'continuations.c',
		'corefile.c',
		'cp-abi.c',
		'cp-namespace.c',
		'cp-support.c',
		'cp-valprint.c',
		'd-lang.c',
		'dbxread.c',
		'demangle.c',
		'dictionary.c',
		'disasm.c',
		'dummy-frame.c',
		'dwarf2expr.c',
		'dwarf2-frame.c',
		'dwarf2loc.c',
		'dwarf2read.c',
		'elfread.c',
		'environ.c',
		'eval.c',
		'event-loop.c',
		'event-top.c',
		'exceptions.c',
		'exec.c',
		'expprint.c',
		'filesystem.c',
		'findvar.c',
		'frame.c',
		'frame-base.c',
		'frame-unwind.c',
		'gdbarch.c',
		'gdbtypes.c',
		'infcall.c',
		'infcmd.c',
		'inferior.c',
		'inflow.c',
		'infrun.c',
		'inline-frame.c',
		'interps.c',
		'language.c',
		'linespec.c',
		'jit.c',
		'macrocmd.c',
		'macroexp.c',
		'macroscope.c',
		'macrotab.c',
		'main.c',
		'maint.c',
		'mdebugread.c',
		'mem-break.c',
		'memrange.c',
		'minsyms.c',
		'objc-exp.c',
		'objc-lang.c',
		'objfiles.c',
		'observer.c',
		'osabi.c',
		'parse.c',
		'printcmd.c',
		'progspace.c',
		'psymtab.c',
		'python/python.c',
		'regcache.c',
		'reggroups.c',
		'remote.c',
		'sentinel-frame.c',
		'ser-base.c',
		'ser-pipe.c',
		'ser-tcp.c',
		'serial.c',
		'skip.c',
		'solib.c',
		'source.c',
		'stabsread.c',
		'stack.c',
		'symfile.c',
		'symmisc.c',
		'symtab.c',
		'target.c',
		'target-descriptions.c',
		'thread.c',
		'top.c',
		'tracepoint.c',
		'typeprint.c',
		'ui-file.c',
		'ui-out.c',
		'user-regs.c',
		'utils.c',
		'valarith.c',
		'valops.c',
		'valprint.c',
		'value.c',
		'vec.c',
		'xml-support.c',
		'xml-syscall.c',
		'xml-tdesc.c',
		'wrapper.c',
		'common/buffer.c',
		'common/common-utils.c',
		'common/ptid.c',
		'common/signals.c',
		'common/xml-utils.c',
	]

	xmlfiles = [
		'features/gdb-target.dtd',
		'features/xinclude.dtd',
		'features/library-list.dtd',
		'features/library-list-svr4.dtd',
		'features/osdata.dtd',
		'features/threads.dtd',
		'features/traceframe-info.dtd',
	]

	@EXTRA_SOURCETASKS = [
		#YaccTask.new(self, 'deffilep.y'),
		#YaccTask.new(self, 'ldgram.y'),
		SimpleGenFileTask.new(self, 'build/xml-builtin.c',
			"bash -x features/feature_to_c.sh build/xml-builtin.c #{xmlfiles.join(' ')}",
			['features/feature_to_c.sh'] + xmlfiles),
	]
	@EXTRA_INCLUDES << '.'
	@EXTRA_INCLUDES << 'common'
	@EXTRA_INCLUDES << '../bfd'
	@EXTRA_INCLUDES << '../opcodes'
	if(HOST == :win32)
		@EXTRA_SOURCEFILES += Dir['windows-*.c']
		@EXTRA_SOURCEFILES += Dir['*-windows-*.c']
		@EXTRA_SOURCEFILES << 'ser-mingw.c'
	elsif(HOST == :linux)
		@EXTRA_SOURCEFILES << 'posix-hdep.c'
		@EXTRA_SOURCEFILES << 'ser-unix.c'
		#@EXTRA_SOURCEFILES += Dir['linux-*.c']
		#@EXTRA_SOURCEFILES += Dir['*-linux.c']
	end
	@IGNORED_FILES = [
		#'linux-thread-db.c',
	]
	@EXTRA_CFLAGS << ' -DHAVE_CONFIG_H=1 -DLOCALEDIR=0'
	@EXTRA_CFLAGS << ' -Wno-cast-qual -Wno-error -Wno-all -Wno-extra -Wno-write-strings'
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
		'ser-unix.c' => ' -DHAVE_TERMIOS=1',
		'ser-tcp.c' => ' -DHAVE_SOCKLEN_T=1 -DHAVE_SYS_IOCTL_H=1',
	}
	@LOCAL_LIBS = [
		'bfd',
		'opcodes',
		'libiberty',
	]
	@LIBRARIES = [
		'z',
		'readline',
	]
	if(HOST == :win32)
		@LIBRARIES << 'intl'
	else
		@LOCAL_LIBS << 'intl'
	end
	@NAME = 'gdb'
end

#work.invoke

class AutomakeWork < Work
	include GccVersion
	def gccVersionClass; AutomakeWork; end
	def gcc; "gcc"; end
	def copy(dst, src)
		CopyFileTask.new(self, @bdir+dst, FileTask.new(self, @COMMON_BUILDDIR+src),
			[DirTask.new(self, File.dirname(@bdir+dst))])
	end
end

work = AutomakeWork.new
work.instance_eval do
	set_defaults
	@bdir = '../build/release/'
	@prerequisites = [
		SimpleGenFileTask.new(self, @bdir+'gdb/Makefile', './configure-linux-release.sh',
			['configure-linux-release.sh']),
		copy('bfd/libbfd.a', 'bfd.a'),
		copy('opcodes/libopcodes.a', 'opcodes.a'),
		copy('libiberty/libiberty.a', 'libiberty.a'),
		#copy('readline/libreadline.a', '/usr/lib/libreadline.a'),
	]
	def setup
	end
end

work.invoke

sh 'cd ../build/release/libdecnumber && make'
sh 'cd ../build/release/gdb && make'
sh "cp -uv ../build/release/gdb/gdb #{mosyncdir}/mapip2/"
