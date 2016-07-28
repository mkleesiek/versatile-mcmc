fmcmc_headers = [
    'controller.h',
    'exception.h',
    'logger.h',
    'numeric.h',
    'parameter.h',
    'random.h'
]

fmcmc_sources = [
    'controller.cpp',
    'logger.cpp',
    'parameter.cpp'
]

install_headers( fmcmc_headers, subdir : 'fmcmc')

fmcmc_lib = shared_library( 'fmcmc',
    fmcmc_sources,
    include_directories : fmcmc_inc,
    dependencies : boost_dep,
    install : true)
                        