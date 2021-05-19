project(
    'demo',
    'cpp',
    version : '0.1',
    default_options : 'cpp_std=c++14')
src_files = ['main.cpp', 'source/Wrapper.cpp']

exe = executable('wrapper', src_files, include_directories: include_directories('./headers', './api', './source'))
test('basic', exe)