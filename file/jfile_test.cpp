#include <stdio.h>
#include <string>
#include "file/jfile.hpp"

using namespace std;

void TestCopyFile()
{
    const string from = "/root/from.txt"; 
    const string to = "/root/to.txt";

    int ret = jarvis::jfile::CopyFile(from, to);
    printf("copy ret: %d\n", ret);
}

int main(int argc, char ** argv)
{
#ifdef TEST_COPY_FILE
    TestCopyFile();
#endif
    return 0;
}
