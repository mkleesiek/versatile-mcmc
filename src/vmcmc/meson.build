vmcmc_headers = [
    'algorithm.h',
    'blas.h',
    'exception.h',
    'io.h',
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
    'typetraits.h'
]

vmcmc_sources = [
    'algorithm.cpp',
    'io.cpp',
    'logger.cpp',
    'metropolis.cpp',
    'parameter.cpp',
    'prior.cpp',
    'proposal.cpp',
    'stringutils.cpp'
]

install_headers( vmcmc_headers, subdir : 'vmcmc')

vmcmc_lib = shared_library( 'vmcmc',
    vmcmc_sources,
    include_directories : vmcmc_inc,
    dependencies : [boost_dep, tbb_dep],
    install : true)
                        