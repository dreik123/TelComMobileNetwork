project(
    'demo',
    'cpp',
    version : '0.1',
    default_options : 'cpp_std=c++14')
src_files = ['main.cpp']
subdir('source')
exe = executable('wrapper', src_files, dependencies: my_dep)
