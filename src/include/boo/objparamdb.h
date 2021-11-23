#pragma once

#include <map>
#include <oead/byml.h>

namespace boo
{

class ObjectParameterDatabase
{

public:
    std::map<std::string, std::map<std::string, oead::Byml::Type>> opd;
    std::unordered_map<std::string, std::string> objectLists;
    void generate(std::string StageDataPath);
    std::string generateFromFile(std::string filename);
    void generateFromFile(std::string filename, std::string& stage);
    u8 load(std::string filename);
    u8 save(std::string filename);

    bool loaded = false;

    ObjectParameterDatabase();
    static ObjectParameterDatabase& Get();

    static std::map<oead::Byml::Type, std::string> types_ts;
    static std::map<std::string, oead::Byml::Type> types_st;

};

};