#pragma once

#include <string>

namespace boo
{
    class Randomizer
    {
    public:
        static void RandomizeLZ(bool shines, bool scenarios, std::string path, std::string output, std::string* progress = nullptr);
        static void RandomizeShines(std::string path, std::string output);
        static void RandomizeBGM(std::string path, std::string output);
    };
}