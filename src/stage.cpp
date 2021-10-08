#include <algorithm>
#include <boo/stage.h>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/yaz0.h>

namespace boo
{
    u8 boo::Stage::Load(std::vector<u8> sarc)
    {
        if (sarc.size() < 20) return 0;
        if (sarc[0] == 'Y' && sarc[1] == 'a' && sarc[2] == 'z' && sarc[3] == '0')
        {
            sarc = oead::yaz0::Decompress(sarc);
        }
        if (!(sarc[0] == 'S' && sarc[1] == 'A' && sarc[2] == 'R' && sarc[3] == 'C')) return 1;
        oead::Sarc archive(sarc);

        oead::Byml stagedata;

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

        if (!loaded) return 1;
        
        data.Load(stagedata);
        return 0;
    }

    u8 boo::StageData::Load(oead::Byml& data)
    {
        u32 i = 0;
        for (auto entry = data.GetArray().cbegin(); entry != data.GetArray().cend(); ++entry)
        {
            entries.push_back(StageDataEntry());
            for (auto object_list = data.GetArray()[i].GetHash().cbegin(); object_list != data.GetArray()[i].GetHash().cend(); ++object_list)
            {
                for (auto object = object_list->second.GetArray().cbegin(); object != object_list->second.GetArray().cend(); ++object)
                {
                    Object o;
                    o.Id = object->GetHash().at("Id").GetString();
                    o.IsLinkDest = object->GetHash().at("IsLinkDest").GetBool();
                    o.LayerConfigName = object->GetHash().at("LayerConfigName").GetString();

                    try
                    {
                        o.ModelName = object->GetHash().at("ModelName").GetString();
                    } catch(std::bad_variant_access& e) {o.ModelName = std::string();}

                    o.PlacementFileName = object->GetHash().at("PlacementFileName").GetString();
                    
                    o.Rotate.x = object->GetHash().at("Rotate").GetHash().at("X").GetFloat();
                    o.Rotate.y = object->GetHash().at("Rotate").GetHash().at("Y").GetFloat();
                    o.Rotate.z = object->GetHash().at("Rotate").GetHash().at("Z").GetFloat();

                    o.Scale.x = object->GetHash().at("Scale").GetHash().at("X").GetFloat();
                    o.Scale.y = object->GetHash().at("Scale").GetHash().at("Y").GetFloat();
                    o.Scale.z = object->GetHash().at("Scale").GetHash().at("Z").GetFloat();

                    o.Translate.x = object->GetHash().at("Translate").GetHash().at("X").GetFloat();
                    o.Translate.y = object->GetHash().at("Translate").GetHash().at("Y").GetFloat();
                    o.Translate.z = object->GetHash().at("Translate").GetHash().at("Z").GetFloat();

                    o.UnitConfig.DisplayName = object->GetHash().at("UnitConfig").GetHash().at("DisplayName").GetString();

                    o.UnitConfig.DisplayRotate.x = object->GetHash().at("UnitConfig").GetHash().at("DisplayRotate").GetHash().at("X").GetFloat();
                    o.UnitConfig.DisplayRotate.y = object->GetHash().at("UnitConfig").GetHash().at("DisplayRotate").GetHash().at("Y").GetFloat();
                    o.UnitConfig.DisplayRotate.z = object->GetHash().at("UnitConfig").GetHash().at("DisplayRotate").GetHash().at("Z").GetFloat();

                    o.UnitConfig.DisplayScale.x = object->GetHash().at("UnitConfig").GetHash().at("DisplayScale").GetHash().at("X").GetFloat();
                    o.UnitConfig.DisplayScale.y = object->GetHash().at("UnitConfig").GetHash().at("DisplayScale").GetHash().at("Y").GetFloat();
                    o.UnitConfig.DisplayScale.z = object->GetHash().at("UnitConfig").GetHash().at("DisplayScale").GetHash().at("Z").GetFloat();

                    o.UnitConfig.DisplayTranslate.x = object->GetHash().at("UnitConfig").GetHash().at("DisplayTranslate").GetHash().at("X").GetFloat();
                    o.UnitConfig.DisplayTranslate.y = object->GetHash().at("UnitConfig").GetHash().at("DisplayTranslate").GetHash().at("Y").GetFloat();
                    o.UnitConfig.DisplayTranslate.z = object->GetHash().at("UnitConfig").GetHash().at("DisplayTranslate").GetHash().at("Z").GetFloat();

                    o.UnitConfig.GenerateCategory = object->GetHash().at("UnitConfig").GetHash().at("GenerateCategory").GetString();
                    o.UnitConfig.ParameterConfigName = object->GetHash().at("UnitConfig").GetHash().at("ParameterConfigName").GetString();
                    
                    std::string PlacementTargetFile = object->GetHash().at("UnitConfig").GetHash().at("PlacementTargetFile").GetString();

                    if (PlacementTargetFile == "Design") o.UnitConfig.PlacementTargetFile = boo::StageType::Design;
                    else if (PlacementTargetFile == "Map") o.UnitConfig.PlacementTargetFile = boo::StageType::Map;
                    else if (PlacementTargetFile == "Sound") o.UnitConfig.PlacementTargetFile = boo::StageType::Sound;

                    o.UnitConfigName = object->GetHash().at("UnitConfigName").GetString();

                    try
                    {
                        o.comment = object->GetHash().at("comment").GetString();
                    } catch(std::bad_variant_access& e) {o.comment = std::string();}

                    for (auto param = object->GetHash().cbegin(); param != object->GetHash().cend(); ++param)
                    {
                        if (param->first != "Id" && param->first != "IsLinkDest" && param->first != "LayerConfigName"
                         && param->first != "ModelName" && param->first != "PlacementFileName" && param->first != "Rotate"
                         && param->first != "Scale" && param->first != "Translate" && param->first != "UnitConfig"
                         && param->first != "UnitConfigName" && param->first != "comment" && param->first != "Links")
                        {
                            o.extra_params[param->first] = param->second;
                        }
                    }
                    
                    entries[i].object_lists[object_list->first].objects.push_back(o);
                }
            }
            i++;
        }
        return 0;
    }
}