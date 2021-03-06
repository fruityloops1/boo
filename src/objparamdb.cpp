#include <boo/config.h>
#include <boo/objparamdb.h>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/yaz0.h>
#include <string>
#include <thread>

namespace boo
{

    void boo::ObjectParameterDatabase::generateFromFile(std::string filename, std::string& stage)
    {
        stage = generateFromFile(filename);
    }

    std::string boo::ObjectParameterDatabase::generateFromFile(std::string filename)
    {
        std::ifstream stage_file(filename, std::ios::in | std::ios::binary);
        std::vector<u8> sarc((std::istreambuf_iterator<char>(stage_file)), std::istreambuf_iterator<char>());
        stage_file.close();

        if (sarc.size() < 20) return "";
        if (sarc[0] == 'Y' && sarc[1] == 'a' && sarc[2] == 'z' && sarc[3] == '0')
            sarc = oead::yaz0::Decompress(sarc);
        if (!(sarc[0] == 'S' && sarc[1] == 'A' && sarc[2] == 'R' && sarc[3] == 'C')) return ""; // Skip any non-SARC files
        oead::Sarc archive(sarc);

        oead::Byml stagedata;

        std::string Name;
        bool loaded = false;

        for (oead::Sarc::File file : archive.GetFiles())
        {
            if ((file.name.ends_with("Design.byml") || file.name.ends_with("Map.byml") || file.name.ends_with("Sound.byml")) && file.name != "LightMap.byml")
            {
                std::string temp(file.name);
                if (file.name.ends_with("Design.byml")) temp = temp.substr(0, temp.size() - 11);
                else if (file.name.ends_with("Map.byml")) temp = temp.substr(0, temp.size() - 8);
                else if (file.name.ends_with("Sound.byml")) temp = temp.substr(0, temp.size() - 10);
                Name = std::string(temp);

                stagedata = oead::Byml::FromBinary(file.data);
                if (stagedata.GetType() == oead::Byml::Type::Null) return Name;
                loaded = true;
                break;
            }
        }

        if (!loaded) return "";

        int b = -1;

        for (u32 i = 0; i < stagedata.GetArray().size(); i++)
        {
            b = i;
            for (auto object_list = stagedata.GetArray()[i].GetHash().cbegin(); object_list != stagedata.GetArray()[i].GetHash().cend(); ++object_list)
            {
                for (oead::Byml object : object_list->second.GetArray())
                {
                    std::function<void(oead::Byml&)> rl;
                    rl = [&rl, this, &object_list](oead::Byml& object) {
                        for (auto param = object.GetHash().cbegin(); param != object.GetHash().cend(); ++param)
                        {
                            if (param->first != "Id" && param->first != "IsLinkDest" && param->first != "LayerConfigName"
                             && param->first != "ModelName" && param->first != "PlacementFileName" && param->first != "Rotate"
                             && param->first != "Scale" && param->first != "SrcUnitLayerList" && param->first != "Translate" && param->first != "UnitConfig"
                             && param->first != "UnitConfigName" && param->first != "comment" && param->first != "Links")
                            {
                                opd[object.GetHash().at("UnitConfig").GetHash().at("ParameterConfigName").GetString()][param->first] = param->second.GetType();
                                objectLists[object.GetHash().at("UnitConfig").GetHash().at("ParameterConfigName").GetString()] = object_list->first;
                            }
                        }
                        for (auto h = object.GetHash().at("Links").GetHash().begin(); h != object.GetHash().at("Links").GetHash().end(); ++h)
                        {
                            for (oead::Byml le : h->second.GetArray())
                            {
                                rl(le);
                            }
                            
                        }
                    };
                    rl(object);
                }
            }
        }
        return Name;
    }

    void boo::ObjectParameterDatabase::generate(std::string StageDataPath)
    {
        for (const auto& sf : std::filesystem::directory_iterator(StageDataPath))
        {
            if (sf.is_directory()) continue;
            generateFromFile(sf.path());
        }
        loaded = true;
    }

    std::map<oead::Byml::Type, std::string> boo::ObjectParameterDatabase::types_ts;
    std::map<std::string, oead::Byml::Type> boo::ObjectParameterDatabase::types_st;

    // Basic file format for saving the database

    u8 boo::ObjectParameterDatabase::load(std::string filename)
    {
        std::ifstream opd_file(filename);
        for (std::string entry; std::getline(opd_file, entry);)
        {
            if (entry == "\n") continue;
            std::string ParameterConfigName = entry.substr(0, entry.find(':'));
            entry.erase(0, entry.find(':') + 1);
            std::string param = entry.substr(0, entry.find(':'));
            entry.erase(0, entry.find(':') + 1);
            std::string type = entry.substr(0, entry.find(':'));
            entry.erase(0, entry.find(':') + 1);
            std::string object_list = std::string(entry);
            try
            {
                opd[ParameterConfigName][param] = types_st.at(type);
                objectLists[ParameterConfigName] = object_list;
            } catch(std::out_of_range& e) {opd_file.close(); return 1;}
        }
        opd_file.close();
        loaded = true;
        return 0;
    }

    u8 boo::ObjectParameterDatabase::save(std::string filename)
    {
        std::ofstream opd_file(filename);
        for (auto o = opd.cbegin(); o != opd.cend(); ++o)
        {
            for (auto p = o->second.cbegin(); p != o->second.cend(); ++p)
            {
                try
                {
                    opd_file << o->first << ":" << p->first << ":" << types_ts.at(p->second) << ":" << objectLists[o->first] << "\n";
                } catch(std::out_of_range& e) {opd_file.close(); return 1;}
            }
        }
        opd_file.close();
        return 0;
    }

    boo::ObjectParameterDatabase::ObjectParameterDatabase()
    {
        types_ts[oead::Byml::Type::Null] = "null";
        types_ts[oead::Byml::Type::String] = "string";
        types_ts[oead::Byml::Type::Binary] = "binary";
        types_ts[oead::Byml::Type::Array] = "array";
        types_ts[oead::Byml::Type::Hash] = "hash";
        types_ts[oead::Byml::Type::Bool] = "bool";
        types_ts[oead::Byml::Type::Int] = "int";
        types_ts[oead::Byml::Type::Float] = "float";
        types_ts[oead::Byml::Type::UInt] = "uint";
        types_ts[oead::Byml::Type::Int64] = "long";
        types_ts[oead::Byml::Type::UInt64] = "ulong";
        types_ts[oead::Byml::Type::Double] = "double";

        types_st["null"] = oead::Byml::Type::Null;
        types_st["string"] = oead::Byml::Type::String;
        types_st["binary"] = oead::Byml::Type::Binary;
        types_st["array"] = oead::Byml::Type::Array;
        types_st["hash"] = oead::Byml::Type::Hash;
        types_st["bool"] = oead::Byml::Type::Bool;
        types_st["int"] = oead::Byml::Type::Int;
        types_st["float"] = oead::Byml::Type::Float;
        types_st["uint"] = oead::Byml::Type::UInt;
        types_st["long"] = oead::Byml::Type::Int64;
        types_st["ulong"] = oead::Byml::Type::UInt64;
        types_st["double"] = oead::Byml::Type::Double;
    }

    ObjectParameterDatabase& boo::ObjectParameterDatabase::Get()
    {
        static ObjectParameterDatabase opdb;
        return opdb;
    }

}