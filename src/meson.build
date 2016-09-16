# build shared library
subdir('vmcmc')

# prepare and build executables
vmcmc_apps = [
    'vmcmc-example'
]

foreach appname : vmcmc_apps
  executable(appname, appname + '.cpp',
    include_directories : vmcmc_inc,
    link_with : vmcmc_lib,
    dependencies : [boost_dep],
    install : true,
    install_rpath : final_rpath
  )
endforeach
