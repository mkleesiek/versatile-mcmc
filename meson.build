project('versatile-mcmc', 'cpp',
  version : '0.1.5',
  license : 'GNU GPL v3' ,
  default_options : ['cpp_std=c++11'],
  meson_version : '>=0.25.0' )

project_description = 'A lightweight and extensible Markov Chain Monte Carlo (MCMC) library written in modern C++.'


# verify C++11 support
compiler = meson.get_compiler('cpp')
cpp11_check = '''static_assert(__cplusplus >= 201103L, "No C++11 support.");'''
cpp11_support = compiler.compiles(cpp11_check, name : 'C++11 test')
if not cpp11_support
    error('Your compiler does not support C++11.')
endif

# verify thread_local support for non-POD types
tls_check = '''#include <thread>\n template<class T> class Test { T a = 0; };\n static thread_local Test<int>;'''
tls_support = compiler.compiles(tls_check, name : 'thread_local test')
if not tls_support
    message('Your compiler does not support "thread_local".')
    add_global_arguments('-DNO_TLS', language : 'cpp')
endif

# resolve external dependencies
boost_dep = dependency('boost', version : '>=1.45.0', modules : ['accumulators'])
tbb_dep   = dependency('tbb', required : false)
gtest_dep = dependency('gtest', main : true, required : false)
doxygen   = find_program('doxygen', required : false)


# extract some build options
prefix = get_option('prefix')
libdir = get_option('libdir')
final_rpath = prefix + '/' + libdir
buildtype = get_option('buildtype')

# print install prefix and build type
message( 'Installation prefix: ' + prefix )
message( 'Build type: ' + buildtype )
message( 'Install RPath: ' + final_rpath )


# set global compiler arguments
if tbb_dep.found() and get_option('use_tbb')
    add_global_arguments('-DUSE_TBB', language : 'cpp')
endif

if not buildtype.startswith('debug')
    add_global_arguments('-DNDEBUG', language : 'cpp')
endif

# define include directories
vmcmc_inc = include_directories('src')


# subdirectories
subdir('src')

if doxygen.found()
    subdir('doc')
endif

if gtest_dep.found() and get_option('build_tests')
    subdir('test')
endif


# generate pkg-config files
pkg_mod = import('pkgconfig')
pkg_mod.generate(
    libraries : vmcmc_lib,
    version : meson.project_version(),
    name : 'libvmcmc',
    filebase : 'vmcmc',
    description : project_description
)
