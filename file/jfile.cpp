#include <cstdint>
#include <unistd.h>
#include <fcntl.h>
#include "file/jfile.hpp"

namespace jarvis
{

namespace jfile
{

void CloseStdIO()
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int OpenFile(const std::string & path)
{
    int fd = open(path.c_str(), O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
        return -1;
    return fd;
}

int CloseFile(int fd)
{
    return close(fd);
}

int CopyFile(const std::string & from, const std::string & to)
{
    int fd1 = 0, fd2 = 0;
    if ((fd1 = OpenFile(from)) < 0)
        return -1;
    if ((fd2 = OpenFile(to)) < 0)
    {
        close(fd1);
        return -1;
    }

    char buf[1024];
    int rn = 0;
    while ((rn = read(fd1, buf, 1024)) > 0)
    {
        write(fd2, buf, rn);
    }
    close(fd1);
    close(fd2);
    return 0;
}

int SetCloseExec(int fd)
{
    return fcntl(fd, F_SETFD, 1);
}

}   // namespace jfile

}   // namespace jarvis
