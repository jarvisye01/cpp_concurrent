#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <string.h>
#include "file//jfile.hpp"
#include "util/jconfig.hpp"
#include "util/jstring.hpp"

namespace jarvis
{

FILE* JConfig::file;

int JConfig::Init(const std::string & path)
{
    file = fopen(path.c_str(), "r");
    if (file == NULL)
        return -1;
    return 0;
}

int JConfig::Done()
{
    return fclose(file);
}

uint64_t JConfig::GetInt(const std::string & key)
{
    std::string val = GetKey(key);
    return strtoull(val.c_str(), NULL, 10);
}

std::string JConfig::GetString(const std::string & key)
{
    return GetKey(key);
}

std::string JConfig::GetKey(const std::string & key)
{
    fseek(file, 0, SEEK_SET);

    while (true)
    {
        std::string line;
        char buf[1024], *tmp;
        // get a line
        while ((tmp = fgets(buf, 1024 - 1, file)) != NULL)
        {
            if (strchr(buf, '\n') == NULL)
            {
                line.append(buf);
            }
            else
            {
                strchr(buf, '\n')[0] = '\0';
                line.append(buf);
                break;
            }
        }

        std::string::size_type pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string k(line.substr(0, pos));
        std::string v(line.substr(pos + 1));
        if (k == key)
            return v;
    }

    return std::string("");
}

}   // namespace jarvis
