vmcmc_tests = [
    'blas-test',
    'chain-test',
    'exception-test',
    'io-test',
    'logger-test',
    'math-test',
    'metropolis-test',
    'numeric-test',
    'parameter-test',
    'proposal-test',
    'random-test',
    'stringutils-test',
    'typetraits-test'
]

foreach p : vmcmc_tests
  exe = executable(p, p + '.cpp',
    include_directories : vmcmc_inc,
    link_with : vmcmc_lib,
    dependencies : [gtest_dep]
  )
  test(p, exe,
    valgrind_args : ['--leak-check=full']
  )
endforeach
