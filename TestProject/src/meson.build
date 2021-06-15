my_source = ['./NetConfAgent.cpp', './MobileClient.cpp']
my_include = include_directories(['../include'])
my_lib = static_library('libcustom', sources : my_source, include_directories: my_include)
my_dep = declare_dependency(link_with: my_lib, include_directories: my_include)