#include "Wrapper.h"
namespace wrapper {
    time_t Wrapper::time(std::time_t* tloc) const noexcept
    {
        return std::time(tloc);
    }

    int Wrapper::stat(const char* pathName, struct stat* statBuf) const noexcept
    {
        return ::stat(pathName, statBuf);
    }

    const char* Wrapper::strerror(int errnum) const noexcept
    {
        return std::strerror(errnum);
    }

    DIR* Wrapper::opendir(const char* name) const noexcept
    {
        return ::opendir(name);
    }

    dirent* Wrapper::readdir(DIR* dirp) const noexcept
    {
        return ::readdir(dirp);
    }

    int Wrapper::closedir(DIR* dirp) const noexcept
    {
        return ::closedir(dirp);
    }

    int Wrapper::open(const char* pathname, int flags) const noexcept
    {
        return ::open(pathname, flags);
    }

    int Wrapper::open(const char* pathname, int flags, int permission) const noexcept
    {
        return ::open(pathname, flags, permission);
    }

    int Wrapper::write(int fd, const char* buf, size_t nbytes) const noexcept
    {
        return ::write(fd, buf, nbytes);
    }

    int Wrapper::close(int fd) const noexcept
    {
        return ::close(fd);
    }

    ssize_t Wrapper::read(int32_t fd, void* buf, size_t count) const noexcept
    {
        return ::read(fd, buf, count);
    }

    void Wrapper::sleep(uint32_t time) const noexcept
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

}
