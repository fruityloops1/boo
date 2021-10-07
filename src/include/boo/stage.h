#ifndef STAGE_H
#define STAGE_H

#include <oead/byml.h>
#include <oead/sarc.h>
#include <oead/types.h>
#include <vector>

namespace boo
{

    class StageData
    {
    public:
        u8 Load(oead::Byml& data);
        oead::Byml Save();
    };

    class Stage
    {
    public:
        std::string Name;
        u8 Load(std::vector<u8> sarc);

        StageData data;
    };

}

#endif