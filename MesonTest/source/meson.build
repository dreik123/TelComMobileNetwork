wrapper_source = ['./Wrapper.cpp']
wrapper_include = include_directories(['../headers', '../api'])
wrapper_lib = static_library('libwrapper', sources : wrapper_source, include_directories: wrapper_include)
my_dep = declare_dependency(link_with: wrapper_lib, include_directories: wrapper_include)