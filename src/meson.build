fastMCMC_headers = [
    'fmcmc/controller.h',
    'fmcmc/exceptions.h',
    'fmcmc/logging.h'
]

fastMCMC_sources = [
    'fmcmc/controller.cpp',
    'fmcmc/logging.cpp'
]

install_headers( fastMCMC_headers, subdir : 'fmcmc')

fastMCMC_lib = shared_library( 'fmcmc',
    fastMCMC_sources,
    include_directories : fastMCMC_inc,
    dependencies : boost_dep,
    install : true)
                        
executable('fmcmc', 'fmcmc.cpp',
    include_directories : fastMCMC_inc,
    link_with : fastMCMC_lib )