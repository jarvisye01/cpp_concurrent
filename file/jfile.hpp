#ifndef J_FILE_H
#define J_FILE_H

#include <string>

namespace jarvis
{

namespace jfile
{

void CloseStdIO();

int OpenFile(const std::string & path);

int CloseFile(int fd);

int CopyFile(const std::string & from, const std::string & to);

}   // namespace jfile

}   // namespace jarvis

#endif
