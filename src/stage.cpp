#include <boo/stage.h>
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

        std::stringstream stagedata_byml;
        stagedata_byml << Name << ".byml";

        oead::Byml stagedata;

        for (oead::Sarc::File file : archive.GetFiles())
        {
            if (file.name == stagedata_byml.str())
            {
                stagedata = oead::Byml::FromBinary(file.data);
                break;
            }
        }
        stagedata_byml.str(std::string());

        data.Load(stagedata);

        return 0;
    }

    u8 boo::StageData::Load(oead::Byml &data)
    {
        
        return 0;
    }
}