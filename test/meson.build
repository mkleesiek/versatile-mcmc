fmcmc_tests = [
    'logger-test',
    'metropolis-test',
    'numeric-test',
    'parameter-test',
    'prior-test',
    'proposal-test',
    'random-test',
    'sample-test',
    'stats-test',
    'ublas-test'
]

foreach p : fmcmc_tests
  exe = executable(p, p + '.cpp',
    include_directories : fmcmc_inc,
    link_with : fmcmc_lib,
    dependencies : gtest_dep
  )
  test(p, exe,
    valgrind_args : ['--leak-check=full']
  )
endforeach
