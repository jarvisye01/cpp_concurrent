#ifndef J_FILE_H
#define J_FILE_H

#include <string>

namespace jarvis
{

namespace jfile
{

void CloseStdIO();
int OpenFile(const std::string & path);

}   // namespace jfile

}   // namespace jarvis

#endif