from __future__ import print_function
import os, sys, glob, platform

## Prefix for install(ed) files
if platform.system() != "Windows":
    prefix="/usr/local"
    if not os.path.isdir(prefix):
        print('[ERROR] Cannot find \'prefix\' directory, aka {:}; aborting'.format(prefix), file=sys.stderr)
        sys.exit(1)
else:
    prefix = os.getcwd()

## Library version
lib_version="0.1.0"
## Library name
lib_name="datetime"
## Include dir (following prefix) if any
inc_dir="datetime"
## the rootdir of the project
root_dir=os.path.abspath(os.getcwd())

## get number of CPUs and use for parallel builds
num_cpu = int(os.environ.get('NUM_CPU', 2))
SetOption('num_jobs', num_cpu)
print("running with -j %s" % GetOption('num_jobs'))

AddOption('--cxx',
          dest='cxx',
          type='string',
          nargs=1,
          action='store',
          metavar='CXX',
          help='C++ Compiler',
          default=None)
AddOption('--std',
          dest='std',
          type='string',
          nargs=1,
          action='store',
          metavar='STD',
          help='C++ Standard [11/14/17/20]',
          default='17')
AddOption('--make-check',
          dest='check',
          action='store_true',
          help='Trigger building of test programs',
          default=False)
AddOption('--make-db',
          dest='build_db',
          action='store_true',
          help='Trigger the creation of a compilation database JSON file',
          default=False)

## Source files (for lib)
lib_src_files = glob.glob(r"src/*.cpp")

## Headers (for lib)
hdr_src_files = glob.glob(r"src/*.hpp")

## Compilation flags, aka CXXFLAGS; same for gcc/clang, different for MS VS
## TODO Bugs creep in when using the '-march=native' option. See e.g. test/test_jdepoch_sofa.cpp
DebugCXXFLAGS_P = '-g -pg -Wall -Wextra -Werror -pedantic -W -Wshadow -Winline -Wdisabled-optimization -DDEBUG'
ProductionCXXFLAGS_P = '-Wall -Wextra -Werror -pedantic -W -Wshadow -O2'
## MSVC (Replace Wall with W4)
DebugCXXFLAGS_W = '/std:c++17 /W4 /WX /Od /EHsc /GR /FI -DDEBUG'
ProductionCXXFLAGS_W = '/std:c++17 /W4 /WX /O2 /EHsc /GR /FI'

## Environments ...
denv = Environment(CXXFLAGS = DebugCXXFLAGS_P if platform.system() != "Windows" else DebugCXXFLAGS_W)
## g++ complains about failing to inline functions if we use the '-Winline' here ...
penv = Environment(CXXFLAGS = ProductionCXXFLAGS_P if platform.system() != "Windows" else ProductionCXXFLAGS_W)

## Command line arguments ...
debug = ARGUMENTS.get('debug', 0)

## Construct the build enviroment
env = denv.Clone() if int(debug) else penv.Clone()

## What compiler should we be using ?
if GetOption('cxx') is not None: env['CXX'] = GetOption('cxx')

## Set the C++ standard
cxxstd = GetOption('std')
env.Append(CXXFLAGS = ' --std=c++{}'.format(cxxstd) if platform.system() != "Windows" else ' /std:c++{}'.format(cxxstd))

## Check if we need a compilation database
if GetOption('build_db') is not None and GetOption('build_db'):
  env.Tool('compilation_db')
  cdb = env.CompilationDatabase()
  Alias('cdb', cdb)

## (shared) library ...
vlib = env.SharedLibrary(source=lib_src_files, target=lib_name, CPPPATH=['.'], SHLIBVERSION=lib_version)

## Build ....
env.Alias(target='install', source=env.Install(dir=os.path.join(prefix, 'include', inc_dir), source=hdr_src_files))
env.Alias(target='install', source=env.InstallVersionedLib(dir=os.path.join(prefix, 'lib'), source=vlib))
 
## (Unit)  Testing
test_targets = []
test_sources = glob.glob(r"test/unit_tests/*.cpp")
for tsource in test_sources:
  ttarget = os.path.join(os.path.dirname(tsource), os.path.basename(tsource).replace('_', '-').replace('.cpp', '.out'))
  test_targets.append(ttarget)
  env.Program(target=ttarget, source=tsource, CXXFLAGS=env['CXXFLAGS']+' -Wl,-rpath,{}'.format(root_dir), LIBS=[lib_name,'sofa_c'], LIBPATH=root_dir, CPPPATH=os.path.join(root_dir,'src'), RPATH=root_dir)

## Tests ... if any assertion fails, this build will fail
if GetOption('check') is not None and GetOption('check'):
  
  tests_sources = glob.glob(r"test/*.cpp")
  env.Append(RPATH=root_dir)
  for tsource in tests_sources:
    ttarget = tsource.replace('_', '-').replace('.cpp', '.out')
    env.Program(target=ttarget, source=tsource, CPPPATH='src/',
                LIBS=vlib, LIBPATH='.')
    #Command(target=ttarget+'-run', source=ttarget, action=ttarget)
  
  tests_targets = []
  tests_sources = glob.glob(r"test/unit_tests/*.cpp")
  for tsource in tests_sources:
    ttarget = os.path.join(os.path.dirname(tsource), os.path.basename(tsource).replace('_', '-').replace('.cpp', '.out'))
    tests_targets.append(ttarget)
  env.Append(RPATH=root_dir)
  program = env.Program(target="check", source=test_sources, LIBS=lib_name+['sofa_c'], LIBPATH=root_dir)
  #for tsource in tests_sources:
  #  ttarget = os.path.join(os.path.dirname(tsource), os.path.basename(tsource).replace('_', '-').replace('.cpp', '.out'))
  #  env.Program(target=ttarget, source=tsource+[lib_src_files], CPPPATH='src/',
  #              LIBS=lib_name+['sofa_c'], LIBPATH=root_dir)
  #  #Command(target=ttarget+'-run', source=ttarget, action=ttarget)
