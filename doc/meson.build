conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('description', project_description)
conf_data.set('source_dir', meson.source_root() + '/src')
conf_data.set('build_dir', meson.build_root() + '/doc')
conf_data.set('install_dir', meson.build_root() + '/doc')

configure_file(input : 'Doxyfile.in', output : 'Doxyfile', configuration : conf_data)

run_target('doc', 'doxygen', meson.current_build_dir() + '/Doxyfile')
