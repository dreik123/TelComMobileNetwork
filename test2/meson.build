project('SysrepoListener', 'cpp',
  version : '1.0',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

#dep = dependency('sysrepo', required : true)
#dep1 = dependency('libyang', required : true)
sysrepo = dependency('sysrepo')
sysrepo_cpp = dependency('sysrepo-cpp')
libyang = dependency('libyang-cpp')

exe = executable('Listener2', 
sources : 'main.cpp',
dependencies : [sysrepo, sysrepo_cpp, libyang], 
  install : true)
  
test('basic', exe)