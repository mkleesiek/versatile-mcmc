project('imgui', 'cpp',
  version : '1.50',
  license : 'MIT' ,
  default_options : ['cpp_std=c++11'],
  meson_version : '>=0.25.0' )


# resolve external dependencies
gl_dep     = dependency('gl')
glfw_dep   = dependency('glfw3')


imgui_inc = include_directories('.')

imgui_lib = library('imgui',
    ['imgui.cpp', 'imgui_draw.cpp', 'imgui_demo.cpp', 'imgui_impl_glfw.cpp' ],
    include_directories : imgui_inc,
    dependencies : [gl_dep, glfw_dep],
    install : true
)
