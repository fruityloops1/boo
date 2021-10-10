#include <boo/objparamdb.h>
#include <filesystem>
#include <fstream>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/yaz0.h>
#include <string>

namespace boo
{

    void boo::ObjectParameterDatabase::Generate(std::string StageDataPath)
    {
        for (const auto& sf : std::filesystem::directory_iterator(StageDataPath))
        {
            if (sf.is_directory()) continue;
            std::ifstream stage_file(sf.path(), std::ios::in | std::ios::binary);
            std::vector<u8> sarc((std::istreambuf_iterator<char>(stage_file)), std::istreambuf_iterator<char>());

            if (sarc.size() < 20) continue;
            if (sarc[0] == 'Y' && sarc[1] == 'a' && sarc[2] == 'z' && sarc[3] == '0')
            {
                sarc = oead::yaz0::Decompress(sarc);
            }
            if (!(sarc[0] == 'S' && sarc[1] == 'A' && sarc[2] == 'R' && sarc[3] == 'C')) continue;
            oead::Sarc archive(sarc);

            oead::Byml stagedata;

            std::string Name;
            bool loaded = false;

            for (oead::Sarc::File file : archive.GetFiles())
            {
                if (file.name.ends_with("Design.byml") || file.name.ends_with("Map.byml") || file.name.ends_with("Sound.byml"))
                {
                    std::string temp(file.name);
                    if (file.name.ends_with("Design.byml")) temp = temp.substr(0, temp.size() - 11);
                    else if (file.name.ends_with("Map.byml")) temp = temp.substr(0, temp.size() - 8);
                    else if (file.name.ends_with("Sound.byml")) temp = temp.substr(0, temp.size() - 10);
                    Name = std::string(temp);

                    stagedata = oead::Byml::FromBinary(file.data);
                    loaded = true;
                    break;
                }
            }

            if (!loaded) continue;

            try
            {
            for (u32 i = 0; i < stagedata.GetArray().size(); i++)
            {
                for (auto object_list = stagedata.GetArray()[i].GetHash().cbegin(); object_list != stagedata.GetArray()[i].GetHash().cend(); ++object_list)
                {
                    for (oead::Byml object : object_list->second.GetArray())
                    {
                        std::function<void(oead::Byml)> rl;
                        rl = [&rl, this](oead::Byml object) {
                            for (auto param = object.GetHash().cbegin(); param != object.GetHash().cend(); ++param)
                            {
                                if (param->first != "Id" && param->first != "IsLinkDest" && param->first != "LayerConfigName"
                                 && param->first != "ModelName" && param->first != "PlacementFileName" && param->first != "Rotate"
                                 && param->first != "Scale" && param->first != "Translate" && param->first != "UnitConfig"
                                 && param->first != "UnitConfigName" && param->first != "comment" && param->first != "Links")
                                {
                                    opd[object.GetHash().at("UnitConfig").GetHash().at("ParameterConfigName").GetString()][param->first] = param->second.GetType();
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
            } catch (std::bad_variant_access& e) {} // Unknown error, doesn't lose any entries since it only happens after the last loop ends
        }
    }

    std::map<oead::Byml::Type, std::string> boo::ObjectParameterDatabase::types_ts;
    std::map<std::string, oead::Byml::Type> boo::ObjectParameterDatabase::types_st;

    u8 boo::ObjectParameterDatabase::Load(std::string filename)
    {
        std::ifstream opd_file(filename);
        for (std::string entry; std::getline(opd_file, entry);)
        {
            if (entry == "\n") continue;
            std::string ParameterConfigName = entry.substr(0, entry.find(':'));
            entry.erase(0, entry.find(':') + 1);
            std::string param = entry.substr(0, entry.find(':'));
            entry.erase(0, entry.find(':') + 1);
            std::string type = std::string(entry);
            try
            {
                opd[ParameterConfigName][param] = types_st.at(type);
            } catch(std::out_of_range& e) {opd_file.close(); return 1;}
        }
        opd_file.close();
        return 0;
    }

    u8 boo::ObjectParameterDatabase::Save(std::string filename)
    {
        std::ofstream opd_file(filename);
        for (auto o = opd.cbegin(); o != opd.cend(); ++o)
        {
            for (auto p = o->second.cbegin(); p != o->second.cend(); ++p)
            {
                try
                {
                    opd_file << o->first << ":" << p->first << ":" << types_ts.at(p->second) << "\n";
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

}