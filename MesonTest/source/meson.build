wrapper_headers = ['../api/IWrapper.h', '../headers/Wrapper.h']
wrapper_source = ['./Wrapper.cpp']
wrapper_lib = static_library('libwrapper', wrapper_source, include_directories: include_directories('../headers', '../api'))
my_dep = declare_dependency(link_with: wrapper_lib)