#ifndef OBJPARAMDB_H
#define OBJPARAMDB_H

#include <filesystem>
#include <map>
#include <oead/byml.h>

namespace boo
{

class ObjectParameterDatabase
{
public:
    std::map<std::string, std::map<std::string, oead::Byml::Type>> opd;
    void Generate(std::string StageDataPath);
};

};

#endif