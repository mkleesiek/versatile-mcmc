# build shared library

subdir('fmcmc')


# prepare and build executables

fmcmc_apps = [
    ['fmcmc-example',   'fmcmc_example.cpp']
]

foreach a : fmcmc_apps
  exe = executable(a.get(0), a.get(1),
    include_directories : fmcmc_inc,
    link_with : fmcmc_lib
  )
endforeach
