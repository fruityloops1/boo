#pragma once

#include <map>
#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/types.h>
#include <vector>

namespace boo
{

    enum StageType
    {
        All, Design, Map, Sound
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
        std::vector<std::pair<std::string, std::string>> SrcUnitLayerList;
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
        u8 load(oead::Byml& data);
        oead::Byml save();

        boo::Object* findObject(std::string Id, int scenario);
        bool deleteObject(std::string Id, int scenario);

        std::vector<StageDataEntry> entries;
    };

    class Stage
    {
    public:
        std::string name;
        u8 load(std::vector<u8> sarc);
        std::vector<u8> save(bool compress);

        StageData data;
        StageType type;
        std::map<std::string, std::vector<u8>> level_archive;
    };

}