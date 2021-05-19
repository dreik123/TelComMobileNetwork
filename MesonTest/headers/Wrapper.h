#include "IWrapper.h"

namespace wrapper {
    class Wrapper final : public IWrapper
    {
    public:
        time_t time(std::time_t* tloc) const noexcept override;

        int stat(const char* pathName, struct stat* statBuf) const noexcept override;

        const char* strerror(int errnum) const noexcept override;

        DIR* opendir(const char* name) const noexcept override;

        dirent* readdir(DIR* dirp) const noexcept override;

        int closedir(DIR* dirp) const noexcept override;

        int open(const char* pathname, int flags) const noexcept override;

        int open(const char* pathname, int flags, int permission) const noexcept override;

        int write(int fd, const char* buf, size_t nbytes) const noexcept override;

        int close(int fd) const noexcept override;

        ssize_t read(int32_t fd, void* buf, size_t count) const noexcept override;

        void sleep(uint32_t time) const noexcept override;
    };
}
