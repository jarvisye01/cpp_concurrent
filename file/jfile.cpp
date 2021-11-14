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
    int fd = open(path.c_str(), O_CREAT | O_RDWR | O_APPEND | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
        return -1;
    return fd;
}

}   // namespace jfile

}   // namespace jarvis
