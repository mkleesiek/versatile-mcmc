project('FastMCMC', 'cpp',
  version : '0.0.1',
  license : 'MIT' ,
  default_options : ['cpp_std=c++11'] )

boost_dep = dependency('boost', version : '>=1.45.0', modules : ['accumulators', 'log'])
gtest_dep = dependency('gtest', main : true)
tbb_dep = dependency('tbb', required : false)

compiler = meson.get_compiler('cpp')
cpp11_check = '''static_assert(__cplusplus >= 201103L, "No C++11 support.");'''
cpp11_support = compiler.compiles(cpp11_check, name : 'C++11 check')

if tbb_dep.found()
    add_global_arguments('-DUSE_TBB', language : 'cpp')
endif

fastMCMC_inc = include_directories('src')

subdir('src')
subdir('test')
subdir('doc')

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : fastMCMC_lib,
                 version : '0.1',
                 name : 'libfmcmc',
                 filebase : 'fmcmc',
                 description : 'The FastMCMC C++ library.')