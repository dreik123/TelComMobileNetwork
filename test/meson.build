project('SysrepoListener', 'cpp',
  version : '1.0',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

dep = dependency('sysrepo', required : true)
dep1 = dependency('libyang', required : true)

exe = executable('Listener', 
sources : 'main.cpp',
dependencies : [dep, dep1], 
  install : true)
  
test('basic', exe)