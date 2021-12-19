#include <cstdint>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include "file/jfile.hpp"
#include "util/jbuffer.hpp"
#include "util/jstring.hpp"
#include "util/jrand.hpp"
#include "util/jtime.hpp"

using namespace std;

void FillFile(string path, int sz)
{
    int fd = jarvis::jfile::OpenFile(path);
    int n = 0;
    while (n < sz)
    {
        int k = std::min(sz - n, 1000);
        string str = jarvis::jstring::GetRandomString(k);
        write(fd, str.c_str(), str.size());
        n += k;
    }
    jarvis::jfile::CloseFile(fd);
}

int main(int argc, char ** argv)
{
    // path1 and path2 are new file
    string path1 = "/root/file1.txt", path2 = "/root/file2.txt";
    int sz = 1024 * 1024 * 10;
    FillFile(path1, sz);

    uint64_t t1 = jarvis::GetMillTime();
    int fd1 = jarvis::jfile::OpenFile(path1);
    jarvis::jutil::JFileBuffer buffer(fd1);
    buffer.ReadFrom(sz);
    jarvis::jfile::CloseFile(fd1);
    uint64_t t2 = jarvis::GetMillTime();
    printf("read from file: %lums\n", t2 - t1);

    int fd2 = jarvis::jfile::OpenFile(path2);
    buffer.SetHandle(fd2);
    printf("buffer size: %d\n", buffer.Size());
    buffer.WriteTo(buffer.Size());
    jarvis::jfile::CloseFile(fd2);
    uint64_t t3 = jarvis::GetMillTime();
    printf("write to file: %lums\n", t3 - t2);
    return 0;
}
