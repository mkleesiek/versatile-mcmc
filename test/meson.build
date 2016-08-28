vmcmc_tests = [
    'blas-test',
    'io-test',
    'logger-test',
    'metropolis-test',
    'numeric-test',
    'parameter-test',
    'prior-test',
    'proposal-test',
    'random-test',
    'stats-test'
]

foreach p : vmcmc_tests
  exe = executable(p, p + '.cpp',
    include_directories : vmcmc_inc,
    link_with : vmcmc_lib,
    dependencies : gtest_dep
  )
  test(p, exe,
    valgrind_args : ['--leak-check=full']
  )
endforeach
