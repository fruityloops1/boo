#include <boo/stage.h>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/yaz0.h>
#include <sstream>

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

        if (Name.empty()) return 2;

        std::stringstream stagedata_bymlss;
        stagedata_bymlss << Name << "Map.byml";
        std::string stagedata_byml = stagedata_bymlss.str();

        oead::Byml stagedata;

        for (oead::Sarc::File file : archive.GetFiles())
        {
            if (file.name == stagedata_byml)
            {
                stagedata = oead::Byml::FromBinary(file.data);
                break;
            }
        }

        data.Load(stagedata);
        return 0;
    }

    u8 boo::StageData::Load(oead::Byml &data)
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
                    o.UnitConfigName = object->GetHash().at("UnitConfigName").GetString();

                    try
                    {
                        o.comment = object->GetHash().at("comment").GetString();
                    } catch(std::bad_variant_access& e) {o.comment = std::string();}
                    
                    entries[i].object_lists[object_list->first].objects.push_back(o);
                }
            }
            i++;
        }
        return 0;
    }
}