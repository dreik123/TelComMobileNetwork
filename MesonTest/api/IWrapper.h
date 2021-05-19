#include <chrono>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
namespace wrapper {
    class IWrapper
    {
    public:
        virtual ~IWrapper() = default;

        virtual std::time_t time(std::time_t* tloc) const noexcept = 0;

        virtual int stat(const char* pathName,  struct stat *statBuf) const noexcept = 0;

        virtual const char* strerror(int errnum) const noexcept = 0;

        virtual DIR* opendir(const char* name) const noexcept = 0;

        virtual struct dirent* readdir(DIR* dirp) const noexcept = 0;

        virtual int closedir(DIR* dirp) const noexcept = 0;

        virtual int open(const char* pathname, int flags) const noexcept = 0;

        virtual int open(const char* pathname, int flags, int permission) const noexcept = 0;

        virtual int write(int fd, const char* buf, size_t nbytes) const noexcept = 0;

        virtual int close(int fd) const noexcept = 0;

        virtual ssize_t read(int32_t fd, void* buf, size_t count) const noexcept = 0;

        virtual void sleep(uint32_t time) const noexcept = 0;
    };
}
