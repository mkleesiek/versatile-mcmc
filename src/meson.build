# build shared library

subdir('vmcmc')


# prepare and build executables

vmcmc_apps = [
    'vmcmc-example'
]

foreach a : vmcmc_apps
  executable(a, a + '.cpp',
    include_directories : vmcmc_inc,
    link_with : vmcmc_lib,
    install : true,
    install_rpath : final_rpath
  )
endforeach
