#include "Wrapper.h"
#include <iostream>
int main()
{
    wrapper::Wrapper w;
    DIR* dir = w.opendir("~/");
    if(!dir)
    {
        std::cout << w.strerror(1) << std::endl;
        return 1;
    }
    
    struct dirent* entry;
    while((entry = w.readdir(dir)) != NULL)
    { 
        std::cout << entry->d_ino << " " << entry->d_name << " " << entry->d_type << entry->d_reclen << std::endl;
        w.sleep(100);
    }
    w.closedir(dir);
    
    return 0;
}