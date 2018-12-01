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
	import SCons.cpp
	import rtconfig
	
	global BuildOptions
	global Projects
	global Env
	global Rtt_Root
	
	Env = env
	Rtt_Root = os.path.abspath(root_directory)
	
	env.PrependENVPath("PATH", rtconfig.EXEC_PATH)
	#add rtconfig.h path
	env.Append(CPPPATH=[str(Dir('#').abspath)])
	
	#add lib build action
	act = SCons.Action.Action(BuildLibInstallAction, "install compiled library... $TARGET")
	bld = Builder(action=act)
	Env.Append(BUILDERS = {"BuildLib": bld})
	
	#parse rtconfig.h to get used components
	PreProcessor = PatchedPreProcessor()
	try:
		f = file('rtconfig.h', 'r')
		contents = f.read()
		f.close()
	except :
		raise
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
		SetOption('no_exec', 1)
		try:
			rtconfig.CROSS_TOOL, rtconfig.PLATFORM = tgt_dict[tgt_name]
		except KeyError:
			print "unknown target"
			sys.exit(1)
			
	AddOption("--genconfig",
		dest="genconfig",
		action="store_true",
		default=False,
		help="generate .config from rtconfig.h"
	)
	AddOption("--menuconfig",
		dest="menuconfig",
		action="store_true",
		default=False,
		help="make menuconfig for rt-thread bsp"
	)
	if GetOption("menuconfig"):
		from menuconfig import menuconfig
		menuconfig(Rtt_Root)
		exit(0)
	AddOption("--useconfig",
		dest="useconfig",
		type="string",
		help="make rtconfig.h from config file"
	)
	
	configfn = GetOption("useconfig")
	if configfn:
		from menuconfig import mk_rtconfig
		mk_rtconfig(configfn)
		exit(0)
		
	AddOption("--verbose",
		dest="verbose",
		action="store_true",
		default=False,
		help="print verbose info during build"
	)
	
	if not GetOption("verbose"):
		env.Replace(
			ARCOMSTR = "AR $TARGET",
			ASCOMSTR = "AS $TARGET",
			ASPPCOMSTR = "AS $TARGET",
			CCCOMSTR = "CC $TARGET",
			CXXCOMSTR = "CXX $TARGET",
			LINKCOMSTR = "LINK $TARGET"
		)
		
	bsp_vdir = "build"
	kernel_vdir = "build/kernel"
	# board build script
	objs = SConscript("SConscript", variant_dir=bsp_vdir, duplicate=0)
	# include kernel
	objs.extend(SConscript(Rtt_Root + "/src/SConscript", variant_dir=kernel_vdir+"/src", duplicate=0))
	
	if not has_libcpu:
		objs.extend(SConscript(Rtt_Root + "/libcpu/SConscript", variant_dir=kernel_vdir + "/libcpu", duplicate=0))
		
	# include components
	objs.extend(SConscript(Rtt_Root + "/components/SConscript", variant_dir=kernel_vdir + "components", duplicate=0, exports="remove_components"))
	return objs
	
	
def GetDepend(depend):
	building = True
	if type(depend) == type("str"):
		if not BuildOptions.has_key(depend) or BuildOptions[depend] == 0:
			building = False
		elif BuildOptions[depend] != "":
			return BuildOptions[depend]
		return building
		
	for item in depend:
		if item != "":
			if not BuildOptions.has_key(item) or BuildOptions[item] == 0:
				building = False
				
	return building
	
	
def AddDepend(option):
	BuildOptions[option] = 1
	
def DefineGroup(name, src, depend, **parameters):
	global Env
	if not GetDepend(depend):
		return []
		
def GroupLibName(name ,env):
	import rtconfig
	return name + "_gcc"
	
def GroupLibFullName(name, env):
	return env['LIBPREFIX'] + GroupLibName(name, env) + env['LIBPREFIX']
	
def BuildLibInstallAction(target, source, env):
	lib_name = GetOption("buildlib")
	for Group in Projects:
		if Group['name'] == lib_name:
			lib_name = GroupLibFullName(Group['name'], env)
			dst_name = os.path.join(Group['path'], lib_name)
			print "copy %s -> %s " % (lib_name, dst_name)
			do_copy_file(lib_name, dst_name)
			break
			
def DoBuilding(target, objects):
	def one_list(l):
		lst= []
		for item in l:
			if type(item) == type([]):
				lst += one_list(item)
			else:
				lst.append(item)
		return lst
		
	def local_group(group, objects):
		if group.has_key("LOCAL_CCFLAGS") or group.has_key("LOCAL_CPPPATH") or group.has_key("LOCAL_CPPDEFINES"):
			CCFLAGS = Env.get("CCFLAGS", "") + group.get("LOCAL_CCFLAGS", "")
			CPPPATH = Env.get("CPPPATH", [""]) + group.get("LOCAL_CPPPATH", [""])
			CPPDEFINES = Env.get("CPPDEFINES", ['']) + group.get("LOCAL_CPPDEFINES", [''])
			
			for source in group['src']:
				objects.append(Env.Object(source, CCFLAGS=CCFLAGS, CPPPATH=CPPPATH, CPPDEFINES=CPPDEFINES))
				
			return True
		return False
		
		
		
		
	objects = one_list(objects)
	program = None
	
	lib_name = GetOption("buildlib")
	if lib_name:
		objects = [] # remove all objects
		for Group in Projects:
			if Group['name'] == lib_name:
				lib_name = GroupLibName(Group['name'], Env)
				if not local_group(Group, objects):
					objects = Env.Object(Group['src'])
					
				program = Env.Library(lib_name, objects)
				Env.BuildLib(lib_name, program)
				
				break
	else:
		for group in Projects:
			if group.has_key("LOCAL_CCFLAGS") or group.has_key("LOCAL_CPPPATH") or group.has_key("LOCAL_CPPDEFINES"):
				for source in group['src']:
					for obj in objects:
						if source.abspath == obj.abspath or (len(obj.sources)>0 and source.abspath == obj.sources[0].abspath):
							objects.remove(obj)
							
		for group in Projects:
			local_group(group, objects)
			
		program = Env.Program(target, objects)
		
	EndBuilding(target, program)
	
	
def EndBuilding(target, program=None):
	import rtconfig
	Env.AddPostAction(target, rtconfig.POST_ACTION)
	
	if GetOption('target') == "ua":
		from ua import PrepareUA
		PrepareUA(Projects, Rtt_Root, str(Dir("#")))
		
	BSP_ROOT = Dir("#").abspath
	
def SrcRemove(src, remove):
	if not src:
		return 
	for item in src:
		if type(item) == type('str'):
			if os.path.basename(item) in remove:
				src.remove(item)
		else:
			if os.path.basename(item.rstr()) in remove:
				src.remove(item)
				
def GetVersion():
	pass
	
	
			