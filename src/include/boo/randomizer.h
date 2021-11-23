#pragma once

#include <string>

namespace boo
{
    class Randomizer
    {
    public:
        static void randomizeLZ(bool shines, bool scenarios, std::string path, std::string output, std::string* progress = nullptr);
        static void randomizeShines(std::string path, std::string output);
        static void randomizeBGM(std::string path, std::string output);
    };
}