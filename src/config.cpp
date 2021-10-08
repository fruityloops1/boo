#include <boo/config.h>

namespace boo
{

    u8 boo::Config::Load(std::string file)
    {
        INIReader reader(file);
        if (reader.ParseError() != 0) return 1;
        StageDataPath = reader.Get("config", "StageData", "");
        ObjectDataPath = reader.Get("config", "ObjectData", "");
        return 0;
    }

    u8 boo::Config::Save(std::string file)
    {
        std::ofstream ini(file);
        ini << "[config]\n";
        ini << "StageData=" << StageDataPath << "\n";
        ini << "ObjectData=" << ObjectDataPath << "";
        ini.close();
        if (!ini) return 1;
        return 0;
    }

}