#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <INIReader.h>
#include <oead/types.h>
#include <string>

namespace boo
{

struct Config
{
    u8 Load(std::string file);
    u8 Save(std::string file);

    std::string StageDataPath;
    std::string ObjectDataPath;
};

};

#endif