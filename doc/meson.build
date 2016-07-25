doxygen = find_program('doxygen', required : false)

if doxygen.found()
    run_target('doc', ['doxygen', 'Doxyfile'])
endif