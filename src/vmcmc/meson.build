vmcmc_headers = [
    'algorithm.hpp',
    'blas.hpp',
    'chain.hpp',
    'exception.hpp',
    'io.hpp',
    'logger.hpp',
    'math.hpp',
    'metropolis.hpp',
    'numeric.hpp',
    'parameter.hpp',
#    'prior.hpp',
    'proposal.hpp',
    'random.hpp',
    'sample.hpp',
    'stringutils.hpp',
    'typetraits.hpp'
]

vmcmc_sources = [
    'algorithm.cpp',
    'chain.cpp',
    'io.cpp',
    'logger.cpp',
    'math.cpp',
    'metropolis.cpp',
    'parameter.cpp',
#    'prior.cpp',
    'proposal.cpp',
    'stringutils.cpp'
]

install_headers( vmcmc_headers, subdir : 'vmcmc' )

vmcmc_lib = shared_library( 'vmcmc',
    vmcmc_sources,
    include_directories : vmcmc_inc,
    dependencies : [boost_dep, tbb_dep, plot_dep],
    install : true )
                        