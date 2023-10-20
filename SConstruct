from __future__ import print_function
import os, sys, glob, platform
import json

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
denv = Environment(CXXFLAGS='-g -pg -Wall -Wextra -Werror -pedantic -W -Wshadow -Winline -Wdisabled-optimization -DDEBUG')
penv = Environment(CXXFLAGS='-Wall -Wextra -Werror -pedantic -W -Wshadow -Winline -O2 -march=native')

## Command line arguments ...
debug = ARGUMENTS.get('debug', 0)
test  = ARGUMENTS.get('test', 0)
sofa  = ARGUMENTS.get('test-vs-sofa', 0)

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
 
## Unit Tests ... 
if test:
    print('Compiling unit test ...')
    tenv = env.Clone()
    tenv['CXXFLAGS'] = ' '.join([ x for x in env['CXXFLAGS'].split() if 'inline' not in x])
    cmp_error_fn = 'test/unit_tests/test_compilation_error.json'
    cerror_dct = {}
    if os.path.isfile(cmp_error_fn): os.remove(cmp_error_fn)
    test_sources = glob.glob(r"test/unit_tests/*.cpp")
    test_sources += glob.glob(r"test/precision/*.cpp")
    tenv.Append(RPATH=root_dir)
    for tsource in test_sources:
        ttarget = os.path.join(os.path.dirname(tsource), os.path.basename(tsource).replace('_', '-').replace('.cpp', '.out'))
        if 'mock' in os.path.basename(tsource):
            cerror_dct[os.path.basename(tsource)] = {
                'name': '{:}'.format(os.path.abspath(tsource)),
                'cxx': '{:}'.format(tenv['CXX']),
                'incp' : '{:}'.format(os.path.abspath(os.path.join(tenv['RPATH'], 'src'))),
                'flags': '{:}'.format(' '.join(['-o', tenv['CXXFLAGS']])), 
                'exit': 1}
        else:
            # print('adding target {:}'.format(ttarget))
            tenv.Program(target=ttarget, source=tsource, CPPPATH='src/', LIBS=vlib, LIBPATH='.')
    with open(cmp_error_fn, 'w') as fjson: print(json.dumps(cerror_dct, indent = 4), file=fjson)

## SOFA Tests ... 
if sofa:
    print('Compiling tests against SOFA')
    tenv = env.Clone()
    tenv['CXXFLAGS'] = ' '.join([ x for x in env['CXXFLAGS'].split() if 'inline' not in x])
    test_sources = glob.glob(r"test/sofa/*.cpp")
    tenv.Append(RPATH=root_dir)
    for tsource in test_sources:
        ttarget = os.path.join(os.path.dirname(tsource), os.path.basename(tsource).replace('_', '-').replace('.cpp', '.out'))
        tenv.Program(target=ttarget, source=tsource, CPPPATH='src/', LIBS=vlib+['sofa_c'], LIBPATH='.')
