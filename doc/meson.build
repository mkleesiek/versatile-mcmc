# prepare variables used for replacement in doxygen configuration
conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('description', project_description)
conf_data.set('source_root', meson.source_root())
conf_data.set('build_dir', meson.current_build_dir())
conf_data.set('install_dir', meson.current_build_dir())

# configure the doxygen configuration file
configure_file(
    input : 'Doxyfile.in',
    output : 'Doxyfile',
    configuration : conf_data
)

# define a new top-level build target
#custom_target('doc',
#   output : ['html', 'latex'],
#   input : meson.current_build_dir() + '/Doxyfile',
#   command : [doxygen, '@INPUT@'],
#   install : true,
#   install_dir : 'doc'
#)

# define a run target
run_target('doc', 'doxygen', meson.current_build_dir() + '/Doxyfile')
