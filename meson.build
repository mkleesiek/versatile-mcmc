project('fast-mcmc', 'cpp',
  version : '0.0.1',
  license : 'GNU GPL v3' ,
  default_options : ['cpp_std=c++11'] )

# verify C++11 support
compiler = meson.get_compiler('cpp')
cpp11_check = '''static_assert(__cplusplus >= 201103L, "No C++11 support.");'''
cpp11_support = compiler.compiles(cpp11_check, name : 'C++11 check')
if not cpp11_support
    error('Your compiler does not support C++11.')
endif

# resolve external dependencies
boost_dep = dependency('boost', version : '>=1.45.0', modules : ['accumulators', 'log'])
tbb_dep = dependency('tbb', required : false)
gtest_dep = dependency('gtest', main : true, required : false)

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

if buildtype.startswith('debug')
    add_global_arguments('-DDEBUG', language : 'cpp')
endif

# define include directories
fmcmc_inc = include_directories('src')

# subdirectories

subdir('src')
subdir('doc')

if gtest_dep.found() and get_option('build_tests')
    subdir('test')
endif

# generate pkg-config files
pkg_mod = import('pkgconfig')
pkg_mod.generate(
    libraries : fmcmc_lib,
    version : '0.1',
    name : 'libfmcmc',
    filebase : 'fmcmc',
    description : 'The fast-mcmc C++ library.'
)