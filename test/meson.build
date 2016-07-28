fmcmc_tests = [
    ['controller-test',   'controller_test.cpp'],
    ['logger-test',       'logger_test.cpp'],
    ['numeric-test',      'numeric_test.cpp'],
    ['parameter-test',    'parameter_test.cpp'],
    ['random-test',       'random_test.cpp']
]

foreach p : fmcmc_tests
  exe = executable(p.get(0), p.get(1),
    include_directories : fmcmc_inc,
    link_with : fmcmc_lib,
    dependencies : gtest_dep
  )
  test(p.get(0), exe)
endforeach
