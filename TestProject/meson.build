project('MobileNetwork', 'cpp',
  version : '1.0',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

sysrepo = dependency('sysrepo')
sysrepo_cpp = dependency('sysrepo-cpp')
libyang = dependency('libyang-cpp')
subdir('src')
exe = executable('MobileNetwork', 
sources : 'main.cpp',
dependencies : [sysrepo, sysrepo_cpp, libyang, my_dep], 
  install : true)