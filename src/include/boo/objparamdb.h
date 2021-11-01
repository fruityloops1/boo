#pragma once

#include <map>
#include <oead/byml.h>

namespace boo
{

class ObjectParameterDatabase
{

public:
    std::map<std::string, std::map<std::string, oead::Byml::Type>> opd;
    void Generate(std::string StageDataPath);
    std::string GenerateFromFile(std::string filename);
    void GenerateFromFile(std::string filename, std::string& stage);
    u8 Load(std::string filename);
    u8 Save(std::string filename);

    bool loaded = false;

    ObjectParameterDatabase();
    static ObjectParameterDatabase& Get();

    static std::map<oead::Byml::Type, std::string> types_ts;
    static std::map<std::string, oead::Byml::Type> types_st;

};

};