from __future__ import print_function
import os, sys, glob, platform

def make_executable(path):
    mode = os.stat(path).st_mode
    mode |= (mode & 0o444) >> 2
    os.chmod(path, mode)

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

## Source files (for lib)
lib_src_files = glob.glob(r"src/*.cpp")

## Headers (for lib)
hdr_src_files = glob.glob(r"src/*.hpp")

## Environments ...
denv = Environment(CXXFLAGS='-std=c++17 -g -pg -Wall -Wextra -Werror -pedantic -W -Wshadow -Winline -Wdisabled-optimization -DDEBUG')
penv = Environment(CXXFLAGS='-std=c++17 -Wall -Wextra -Werror -pedantic -W -Wshadow -Winline -O2 -march=native')

## Command line arguments ...
debug = ARGUMENTS.get('debug', 0)
test  = ARGUMENTS.get('test', 0)

## Construct the build enviroment
env = denv.Clone() if int(debug) else penv.Clone()

## What compiler should we be using ?
if GetOption('cxx') is not None: env['CXX'] = GetOption('cxx')

## Set the C++ standard
cxxstd = GetOption('std')
env.Append(CXXFLAGS=' --std=c++{}'.format(cxxstd))

## (shared) library ...
vlib = env.SharedLibrary(source=lib_src_files, target=lib_name, CPPPATH=['.'], SHLIBVERSION=lib_version)

## Build ....
env.Alias(target='install', source=env.Install(dir=os.path.join(prefix, 'include', inc_dir), source=hdr_src_files))
env.Alias(target='install', source=env.InstallVersionedLib(dir=os.path.join(prefix, 'lib'), source=vlib))
 
## Tests ... 
if test:
    if os.path.isfile('test_error_sources.sh'): os.remove('test_error_sources.sh')
    test_sources = glob.glob(r"test/unit_tests/*.cpp")
    env.Append(RPATH=root_dir)
    with open('test_error_sources.sh', 'w') as fsh:
        print('#! /usr/bin/bash', file=fsh)
        for tsource in test_sources:
            ttarget = os.path.join(os.path.dirname(tsource), os.path.basename(tsource).replace('_', '-').replace('.cpp', '.out'))
            if 'mock' in os.path.basename(tsource):
                print('if {:} -o {:} {:} -I{:} &>/dev/null; then echo "{:}: compiles when it shouldn\'t!"; else echo "{:}: ok"; fi'.format(env['CXX'], env['CXXFLAGS'], tsource, os.path.join(env['RPATH'], 'src'), os.path.basename(tsource), os.path.basename(tsource)), file=fsh)
            else:
                env.Program(target=ttarget, source=tsource, CPPPATH='src/', LIBS=vlib, LIBPATH='.')
    make_executable('test_error_sources.sh')
