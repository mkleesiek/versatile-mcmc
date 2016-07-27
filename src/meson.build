fmcmc_headers = [
    'fmcmc/controller.h',
    'fmcmc/exception.h',
    'fmcmc/logger.h',
    'fmcmc/numeric.h',
    'fmcmc/parameter.h',
    'fmcmc/random.h'
]

fmcmc_sources = [
    'fmcmc/controller.cpp',
    'fmcmc/logger.cpp',
    'fmcmc/parameter.cpp'
]

install_headers( fmcmc_headers, subdir : 'fmcmc')

fmcmc_lib = shared_library( 'fmcmc',
    fmcmc_sources,
    include_directories : fmcmc_inc,
    dependencies : boost_dep,
    install : true)
                        
executable('fmcmc', 'fmcmc.cpp',
    include_directories : fmcmc_inc,
    link_with : fmcmc_lib )