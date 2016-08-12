fmcmc_headers = [
    'algorithm.h',
    'exception.h',
    'logger.h',
    'metropolis.h',
    'numeric.h',
    'parameter.h',
    'prior.h',
    'proposal.h',
    'random.h',
    'sample.h',
    'stats.h',
    'stringutils.h',
    'typetraits.h',
    'ublas.h'
]

fmcmc_sources = [
    'algorithm.cpp',
    'logger.cpp',
    'metropolis.cpp',
    'parameter.cpp',
    'prior.cpp',
    'proposal.cpp'
]

install_headers( fmcmc_headers, subdir : 'fmcmc')

fmcmc_lib = shared_library( 'fmcmc',
    fmcmc_sources,
    include_directories : fmcmc_inc,
    dependencies : boost_dep,
    install : true)
                        