#pragma once

#include <fstream>
#include <INIReader.h>
#include <oead/types.h>
#include <string>

namespace boo
{

class Config
{
public:
    u8 Load(std::string file);
    u8 Save(std::string file);

    std::string stageDataPath;
    std::string objectDataPath;
    u8 language = 0;
    int fontsize = 16;

    static Config& Get();
};

};