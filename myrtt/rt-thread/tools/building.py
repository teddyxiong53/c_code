import os,sys,string

from SCons.Script import *
from utils import _make_path_relative
from mkdist import do_copy_file


BuildOptions = {}
Projects = []
Rtt_Root = ""
Env = None

def start_handling_includes(self, t=None):
	d = self.dispatch_table
	p = self.stack[-1] if self.stack else self.default_table
	
	for k in ("import", "include", "include_next", "define"):
		d[k] = p[k]
		
def stop_handling_includes(self, t=None):
	d = self.dispatch_table
	d['import'] = self.do_nothing
	d['include'] = self.do_nothing
	d['include_next'] = self.do_nothing
	d['define'] = self.do_nothing
	
	
PatchedPreProcessor = SCons.cpp.PreProcessor
PatchedPreProcessor.start_handling_includes = start_handling_includes
PatchedPreProcessor.stop_handling_includes = stop_handling_includes

def PrepareBuilding(env, root_directory, has_libcpu=False, remove_components=[]):
	import Scons.cpp
	import rtconfig
	
	global BuildOptions
	global Projects
	global Env
	global Rtt_Root
	
	Env = env
	Rtt_Root = os.path.abspath(root_directory)
	
	env.PrependENVPath("PATH", rtconfig.EXEC_PATH)
	#add rtconfig.h path
	env.Append(CPPPATH=[str(Dir('#').abspath])
	
	#add lib build action
	act = SCons.Action.Action(BuildLibInstallAction, "install compiled library... $TARGET")
	bld = Builder(action=act)
	Env.Append(BUILDERS = {"BuildLib": bld})
	
	#parse rtconfig.h to get used components
	PreProcessor = PatchedPreProcessor()
	f = file('rtconfig.h', 'r')
	contents = f.read()
	f.close()
	PreProcessor.process_contents(contents)
	BuildOptions = PreProcessor.cpp_namespace
	
	# add copy option
	AddOption("--copy", dest="copy", action="store_true", 
		default=False,
		help="copy rt-thread dir to local"
	)
	AddOption("--copy-header",
		dest="copy-header",
		action="store_true",
		default=False,
		help="copy header of rt-thread dir to local"
	)
	AddOption("--dist",
		dest="make-dist",
		action="store_true",
		default=False,
		help="make distribution"
	)
	AddOption(
		"--cscope",
		dest="cscope",
		action="store_true",
		default=False,
		help="build cscope cross reference database, require cscope installed"
	)
	AddOption("--buildlib",
		dest="buildlib",
		type="string",
		help="building lib of a component"
	)
	AddOption("--cleanlib",
		dest="cleanlib",
		action="store_true",
		default=False,
		help="clean up the lib "
	)
	AddOption("--target",
		dest="target",
		type="string",
		help="set target project"
	)
	tgt_dict = {
		"ua":('gcc', 'gcc')
	}
	tgt_name = GetOption('target')
	
	if tgt_name:
		