# build shared library

subdir('fmcmc')


# prepare and build executables

fmcmc_apps = [
    'fmcmc-example'
]

foreach a : fmcmc_apps
  executable(a, a + '.cpp',
    include_directories : fmcmc_inc,
    link_with : fmcmc_lib,
    install : true,
    install_rpath : final_rpath
  )
endforeach
