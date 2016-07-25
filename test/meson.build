controllertest_exe = executable('controller-test', 'controller_test.cpp',
    include_directories : fastMCMC_inc,
    link_with : fastMCMC_lib,
    dependencies : gtest_dep )
                     
loggingtest_exe = executable('logging-test', 'logging_test.cpp',
    include_directories : fastMCMC_inc,
    link_with : fastMCMC_lib,
    dependencies : gtest_dep )
                     
test('Controller Test', controllertest_exe)
test('Logging Test', loggingtest_exe)