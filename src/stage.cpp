#include <algorithm>
#include <boo/stage.h>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/types.h>
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
            level_archive[std::string(file.name)] = std::vector<u8>(file.data.data(), file.data.data() + file.data.size());

        for (oead::Sarc::File file : archive.GetFiles())
        {
            if ((file.name.ends_with("Design.byml") || file.name.ends_with("Map.byml") || file.name.ends_with("Sound.byml")) && file.name != "LightMap.byml")
            {
                std::string temp(file.name);
                if (file.name.ends_with("Design.byml"))
                {
                    temp = temp.substr(0, temp.size() - 11);
                    type = boo::StageType::Design;
                }
                else if (file.name.ends_with("Map.byml"))
                {
                    temp = temp.substr(0, temp.size() - 8);
                    type = boo::StageType::Map;
                }
                else if (file.name.ends_with("Sound.byml"))
                {
                    temp = temp.substr(0, temp.size() - 10);
                    type = boo::StageType::Sound;
                }
                Name = std::string(temp);

                stagedata = oead::Byml::FromBinary(file.data);
                if (stagedata.GetType() == oead::Byml::Type::Null) return 3;
                loaded = true;
                break;
            }
        }
        if (!loaded) return 2;
        
        data.Load(stagedata);
        return 0;
    }

    std::vector<u8> boo::Stage::Save(bool compress)
    {
        std::string stagedata_file(Name);
        switch (type)
        {
            case boo::StageType::Design:
            {
                stagedata_file.append("Design.byml");
                break;
            }
            case boo::StageType::Map:
            {
                stagedata_file.append("Map.byml");
                break;
            }
            case boo::StageType::Sound:
            {
                stagedata_file.append("Sound.byml");
                break;
            }
            default: break;
        }

        level_archive[stagedata_file] = data.Save().ToBinary(false, 3);
        oead::SarcWriter writer;
        writer.SetMode(oead::SarcWriter::Mode::New);
        writer.SetEndianness(oead::util::Endianness::Little);
        for (auto file = level_archive.begin(); file != level_archive.end(); ++file)
        {
            writer.m_files[file->first] = file->second;
        }
        std::vector<u8> result = writer.Write().second;
        if (compress)
            return oead::yaz0::Compress(result, 0, 9);
        else return result;
    }

    u8 boo::StageData::Load(oead::Byml& data)
    {
        for (u32 i = 0; i < data.GetArray().size(); i++)
        {
            entries.push_back(StageDataEntry());
            for (auto object_list = data.GetArray()[i].GetHash().begin(); object_list != data.GetArray()[i].GetHash().end(); ++object_list)
            {
                for (oead::Byml object : object_list->second.GetArray())
                {
                    std::function<Object(oead::Byml)> parse;
                    parse = [&parse](oead::Byml object)->Object {
                        Object o;
                        o.Id = object.GetHash().at("Id").GetString();
                        o.IsLinkDest = object.GetHash().at("IsLinkDest").GetBool();
                        o.LayerConfigName = object.GetHash().at("LayerConfigName").GetString();
                    
                        for (auto h = object.GetHash().at("Links").GetHash().begin(); h != object.GetHash().at("Links").GetHash().end(); ++h)
                            for (auto le : h->second.GetArray())
                                o.Links[h->first].push_back(parse(le.GetHash()));
                        try
                        {
                            o.ModelName = object.GetHash().at("ModelName").GetString();
                        } catch(std::bad_variant_access& e) {o.ModelName = std::string();}

                        try
                        {
                            o.PlacementFileName = object.GetHash().at("PlacementFileName").GetString();
                        } catch(std::out_of_range& e) {o.PlacementFileName = std::string();}
                    
                        o.Rotate.x = object.GetHash().at("Rotate").GetHash().at("X").GetFloat();
                        o.Rotate.y = object.GetHash().at("Rotate").GetHash().at("Y").GetFloat();
                        o.Rotate.z = object.GetHash().at("Rotate").GetHash().at("Z").GetFloat();

                        o.Scale.x = object.GetHash().at("Scale").GetHash().at("X").GetFloat();
                        o.Scale.y = object.GetHash().at("Scale").GetHash().at("Y").GetFloat();
                        o.Scale.z = object.GetHash().at("Scale").GetHash().at("Z").GetFloat();

                        try
                        {
                            for (oead::Byml sulle : object.GetHash().at("SrcUnitLayerList").GetArray())
                                o.SrcUnitLayerList.push_back(std::pair<std::string, std::string>(sulle.GetHash().at("LayerName").GetString(), sulle.GetHash().at("LinkName").GetString()));
                        } catch(std::out_of_range& e) {}

                        o.Translate.x = object.GetHash().at("Translate").GetHash().at("X").GetFloat();
                        o.Translate.y = object.GetHash().at("Translate").GetHash().at("Y").GetFloat();
                        o.Translate.z = object.GetHash().at("Translate").GetHash().at("Z").GetFloat();

                        o.UnitConfig.DisplayName = object.GetHash().at("UnitConfig").GetHash().at("DisplayName").GetString();

                            o.UnitConfig.DisplayRotate.x = object.GetHash().at("UnitConfig").GetHash().at("DisplayRotate").GetHash().at("X").GetFloat();
                        o.UnitConfig.DisplayRotate.y = object.GetHash().at("UnitConfig").GetHash().at("DisplayRotate").GetHash().at("Y").GetFloat();
                        o.UnitConfig.DisplayRotate.z = object.GetHash().at("UnitConfig").GetHash().at("DisplayRotate").GetHash().at("Z").GetFloat();

                        o.UnitConfig.DisplayScale.x = object.GetHash().at("UnitConfig").GetHash().at("DisplayScale").GetHash().at("X").GetFloat();
                        o.UnitConfig.DisplayScale.y = object.GetHash().at("UnitConfig").GetHash().at("DisplayScale").GetHash().at("Y").GetFloat();
                        o.UnitConfig.DisplayScale.z = object.GetHash().at("UnitConfig").GetHash().at("DisplayScale").GetHash().at("Z").GetFloat();

                        o.UnitConfig.DisplayTranslate.x = object.GetHash().at("UnitConfig").GetHash().at("DisplayTranslate").GetHash().at("X").GetFloat();
                        o.UnitConfig.DisplayTranslate.y = object.GetHash().at("UnitConfig").GetHash().at("DisplayTranslate").GetHash().at("Y").GetFloat();
                        o.UnitConfig.DisplayTranslate.z = object.GetHash().at("UnitConfig").GetHash().at("DisplayTranslate").GetHash().at("Z").GetFloat();

                        try
                        {
                            o.UnitConfig.GenerateCategory = object.GetHash().at("UnitConfig").GetHash().at("GenerateCategory").GetString();
                        } catch (std::bad_variant_access& e) {o.UnitConfig.GenerateCategory = std::string();} // Metro kingdom mechawiggler rails
                        o.UnitConfig.ParameterConfigName = object.GetHash().at("UnitConfig").GetHash().at("ParameterConfigName").GetString();
                    
                        std::string PlacementTargetFile = object.GetHash().at("UnitConfig").GetHash().at("PlacementTargetFile").GetString();

                        if (PlacementTargetFile == "All") o.UnitConfig.PlacementTargetFile = boo::StageType::All;
                        else if (PlacementTargetFile == "Design") o.UnitConfig.PlacementTargetFile = boo::StageType::Design;
                        else if (PlacementTargetFile == "Map") o.UnitConfig.PlacementTargetFile = boo::StageType::Map;
                        else if (PlacementTargetFile == "Sound") o.UnitConfig.PlacementTargetFile = boo::StageType::Sound;

                        o.UnitConfigName = object.GetHash().at("UnitConfigName").GetString();

                        try
                        {
                            o.comment = object.GetHash().at("comment").GetString();
                        } catch(std::bad_variant_access& e) {o.comment = std::string();}
                        catch(std::out_of_range& e) {o.comment = std::string();}

                        for (auto param = object.GetHash().cbegin(); param != object.GetHash().cend(); ++param)
                        {
                            if (param->first != "Id" && param->first != "IsLinkDest" && param->first != "LayerConfigName"
                             && param->first != "ModelName" && param->first != "PlacementFileName" && param->first != "Rotate"
                             && param->first != "Scale" && param->first != "SrcUnitLayerList" && param->first != "Translate" && param->first != "UnitConfig"
                             && param->first != "UnitConfigName" && param->first != "comment" && param->first != "Links")
                            {
                                o.extra_params[param->first] = param->second;
                            }
                        }
                        return o;
                    };
                    entries[i].object_lists[object_list->first].objects.push_back(parse(object));
                }
            }
        }
        return 0;
    }

    oead::Byml boo::StageData::Save()
    {
        oead::Byml b = oead::Byml(std::vector<oead::Byml>());
        u32 ac = 0;
        for (StageDataEntry& sde : entries)
        {
            b.GetArray().push_back(absl::btree_map<std::string, oead::Byml>());
            for (auto ol = sde.object_lists.begin(); ol != sde.object_lists.end(); ++ol)
            {
                u32 oc = 0;
                b.GetArray()[ac].GetHash()[ol->first] = std::vector<oead::Byml>();
                for (Object o : ol->second.objects)
                {
                    std::function<oead::Byml(Object)> ser;
                    ser = [&ser, &ac, &ol, &oc](Object object)->oead::Byml {
                        oead::Byml b = oead::Byml(absl::btree_map<std::string, oead::Byml>()); 

                        b.GetHash()["Id"] = object.Id;
                        b.GetHash()["IsLinkDest"] = object.IsLinkDest;
                        b.GetHash()["LayerConfigName"] = object.LayerConfigName;

                        b.GetHash()["Links"] = absl::btree_map<std::string, oead::Byml>();
                        for (auto link = object.Links.begin(); link != object.Links.end(); ++link)
                        {
                            b.GetHash()["Links"].GetHash()[link->first] = std::vector<oead::Byml>();
                            for (Object linked_object : link->second)
                                b.GetHash()["Links"].GetHash()[link->first].GetArray().push_back(ser(linked_object));
                        }

                        if (object.ModelName.empty()) b.GetHash()["ModelName"] = oead::Byml();
                        else b.GetHash()["ModelName"] = object.ModelName;
                    
                        b.GetHash()["PlacementFileName"] = object.PlacementFileName;

                        b.GetHash()["Rotate"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["Rotate"].GetHash()["X"] = oead::Number<float>(object.Rotate.x);
                        b.GetHash()["Rotate"].GetHash()["Y"] = oead::Number<float>(object.Rotate.y);
                        b.GetHash()["Rotate"].GetHash()["Z"] = oead::Number<float>(object.Rotate.z);

                        b.GetHash()["Scale"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["Scale"].GetHash()["X"] = oead::Number<float>(object.Scale.x);
                        b.GetHash()["Scale"].GetHash()["Y"] = oead::Number<float>(object.Scale.y);
                        b.GetHash()["Scale"].GetHash()["Z"] = oead::Number<float>(object.Scale.z);

                        if (object.SrcUnitLayerList.size() > 0)
                        {
                            b.GetHash()["SrcUnitLayerList"] = std::vector<oead::Byml>();
                            for (std::pair<std::string, std::string> sull : object.SrcUnitLayerList)
                            {
                                oead::Byml bsull = absl::btree_map<std::string, oead::Byml>();
                                bsull.GetHash()["LayerName"] = sull.first;
                                bsull.GetHash()["LinkName"] = sull.second;
                                b.GetHash()["SrcUnitLayerList"].GetArray().push_back(bsull);
                            }
                        }

                        b.GetHash()["Translate"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["Translate"].GetHash()["X"] = oead::Number<float>(object.Translate.x);
                        b.GetHash()["Translate"].GetHash()["Y"] = oead::Number<float>(object.Translate.y);
                        b.GetHash()["Translate"].GetHash()["Z"] = oead::Number<float>(object.Translate.z);

                        b.GetHash()["UnitConfig"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["UnitConfig"].GetHash()["DisplayName"] = object.UnitConfig.DisplayName;

                        b.GetHash()["UnitConfig"].GetHash()["DisplayRotate"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["UnitConfig"].GetHash()["DisplayRotate"].GetHash()["X"] = oead::Number<float>(object.UnitConfig.DisplayRotate.x);
                        b.GetHash()["UnitConfig"].GetHash()["DisplayRotate"].GetHash()["Y"] = oead::Number<float>(object.UnitConfig.DisplayRotate.y);
                        b.GetHash()["UnitConfig"].GetHash()["DisplayRotate"].GetHash()["Z"] = oead::Number<float>(object.UnitConfig.DisplayRotate.z);

                        b.GetHash()["UnitConfig"].GetHash()["DisplayScale"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["UnitConfig"].GetHash()["DisplayScale"].GetHash()["X"] = oead::Number<float>(object.UnitConfig.DisplayScale.x);
                        b.GetHash()["UnitConfig"].GetHash()["DisplayScale"].GetHash()["Y"] = oead::Number<float>(object.UnitConfig.DisplayScale.y);
                        b.GetHash()["UnitConfig"].GetHash()["DisplayScale"].GetHash()["Z"] = oead::Number<float>(object.UnitConfig.DisplayScale.z);

                        b.GetHash()["UnitConfig"].GetHash()["DisplayTranslate"] = absl::btree_map<std::string, oead::Byml>();
                        b.GetHash()["UnitConfig"].GetHash()["DisplayTranslate"].GetHash()["X"] = oead::Number<float>(object.UnitConfig.DisplayTranslate.x);
                        b.GetHash()["UnitConfig"].GetHash()["DisplayTranslate"].GetHash()["Y"] = oead::Number<float>(object.UnitConfig.DisplayTranslate.y);
                        b.GetHash()["UnitConfig"].GetHash()["DisplayTranslate"].GetHash()["Z"] = oead::Number<float>(object.UnitConfig.DisplayTranslate.z);

                        if (object.UnitConfig.GenerateCategory.empty())
                            b.GetHash()["UnitConfig"].GetHash()["GenerateCategory"] = oead::Byml();
                        else
                            b.GetHash()["UnitConfig"].GetHash()["GenerateCategory"] = object.UnitConfig.GenerateCategory;
                        b.GetHash()["UnitConfig"].GetHash()["ParameterConfigName"] = object.UnitConfig.ParameterConfigName;

                        if (object.UnitConfig.PlacementTargetFile == boo::StageType::All)
                            b.GetHash()["UnitConfig"].GetHash()["PlacementTargetFile"] = std::string("All");
                        else if (object.UnitConfig.PlacementTargetFile == boo::StageType::Design)
                            b.GetHash()["UnitConfig"].GetHash()["PlacementTargetFile"] = std::string("Design");
                        else if (object.UnitConfig.PlacementTargetFile == boo::StageType::Map)
                            b.GetHash()["UnitConfig"].GetHash()["PlacementTargetFile"] = std::string("Map");
                        else if (object.UnitConfig.PlacementTargetFile == boo::StageType::Sound)
                            b.GetHash()["UnitConfig"].GetHash()["PlacementTargetFile"] = std::string("Sound");

                        b.GetHash()["UnitConfigName"] = object.UnitConfigName;
                        if (object.comment.empty()) b.GetHash()["comment"] = oead::Byml();
                        else b.GetHash()["comment"] = object.comment;

                        for (auto ep = object.extra_params.begin(); ep != object.extra_params.end(); ++ep)
                        {
                            b.GetHash()[ep->first] = ep->second;
                        }
                        return b;
                    };
                    b.GetArray()[ac].GetHash()[ol->first].GetArray().push_back(ser(o));
                    oc++;
                }
            }
            ac++;
        }
        return b;
    }

    boo::Object* boo::StageData::FindObject(std::string Id, int scenario)
    {
        boo::Object* found = nullptr;
        for (auto ol = entries[scenario].object_lists.begin(); ol != entries[scenario].object_lists.end(); ++ol)
        {
            for (boo::Object& o : ol->second.objects)
            {
                std::function<void(boo::Object&)> rec;
                rec = [&Id, &rec, &found](boo::Object& fo)
                {
                    if (fo.Id == Id) found = &fo;
                    for (auto loi = fo.Links.begin(); loi != fo.Links.end(); ++loi)
                    {
                        for (boo::Object& lo : loi->second)
                        {
                            rec(lo);
                        }
                    }
                };
                rec(o);
            }
        }
        return found;
    }

    bool boo::StageData::DeleteObject(std::string Id, int scenario)
    {
        for (auto ol = entries[scenario].object_lists.begin(); ol != entries[scenario].object_lists.end(); ++ol)
        {
            auto object = ol->second.objects.begin();
            while (object != ol->second.objects.end())
            {
                std::function<bool(boo::Object&)> rec;
                rec = [&Id, &rec](boo::Object& fo)
                {
                    if (fo.Id == Id) return true;
                    for (auto loi = fo.Links.begin(); loi != fo.Links.end(); ++loi)
                    {
                        auto lo = loi->second.begin();
                        while (lo != loi->second.end())
                        {
                            if (rec(*lo))
                            {
                                loi->second.erase(lo);
                            } else ++lo;
                        }
                    }
                    return false;
                };
                if (rec(*object))
                {
                    ol->second.objects.erase(object);
                    return true;
                } else ++object;
            }
        }
        return false;
    }
}