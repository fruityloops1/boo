#include <boo/objparamdb.h>
#include <filesystem>
#include <fstream>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/yaz0.h>
#include <iostream>
#include <variant>
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

            u32 i = 0;
            try {
            for (auto entry = stagedata.GetArray().begin(); entry != stagedata.GetArray().end(); ++entry)
            {
                for (auto object_list = stagedata.GetArray()[i].GetHash().cbegin(); object_list != stagedata.GetArray()[i].GetHash().cend(); ++object_list)
                {
                    for (auto object = object_list->second.GetArray().cbegin(); object != object_list->second.GetArray().cend(); ++object)
                    {
                        for (auto param = object->GetHash().cbegin(); param != object->GetHash().cend(); ++param)
                        {
                            if (param->first != "Id" && param->first != "IsLinkDest" && param->first != "LayerConfigName"
                             && param->first != "ModelName" && param->first != "PlacementFileName" && param->first != "Rotate"
                             && param->first != "Scale" && param->first != "Translate" && param->first != "UnitConfig"
                             && param->first != "UnitConfigName" && param->first != "comment" && param->first != "Links")
                            {
                                opd[object->GetHash().at("UnitConfig").GetHash().at("ParameterConfigName").GetString()][param->first] = param->second.GetType();
                            }
                        }
                    }
                }
            }
            }catch(...) {}
        }
        
    }

}