#ifndef STAGE_H
#define STAGE_H

#include <map>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/types.h>
#include <vector>

namespace boo
{

    enum StageType
    {
        Design, Map, Sound
    };

    struct UnitConfig
    {
        std::string DisplayName;
        oead::Vector3f DisplayRotate, DisplayScale, DisplayTranslate;
        std::string GenerateCategory;
        std::string ParameterConfigName;
        StageType PlacementTargetFile;
    };

    struct Object
    {
        std::string Id;
        bool IsLinkDest;
        std::string LayerConfigName;
        std::map<std::string, std::vector<Object>> Links;
        std::string ModelName;
        std::string PlacementFileName;
        oead::Vector3f Rotate, Scale, Translate;
        UnitConfig UnitConfig;
        std::string UnitConfigName;
        std::string comment;

        std::map<std::string, oead::Byml> extra_params;
    };

    struct ObjectList
    {
        std::vector<boo::Object> objects;
    };

    struct StageDataEntry
    {
        std::map<std::string, ObjectList> object_lists;
    };

    class StageData
    {
    public:
        u8 Load(oead::Byml& data);
        oead::Byml Save();

        std::vector<StageDataEntry> entries;
    };

    class Stage
    {
    public:
        std::string Name;
        u8 Load(std::vector<u8> sarc);
        std::vector<u8> Save();

        StageData data;
        StageType type;
        std::map<std::string, std::vector<u8>> level_archive;
    };

}

#endif