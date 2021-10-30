#include <boo/config.h>
#include <string>

namespace boo
{

    u8 boo::Config::Load(std::string file)
    {
        INIReader reader(file);
        if (reader.ParseError() != 0) return 1;
        StageDataPath = reader.Get("config", "StageData", "");
        ObjectDataPath = reader.Get("config", "ObjectData", "");
        std::string lang = reader.Get("config", "language", "en");
        std::string fontsizes = reader.Get("config", "fontsize", "18");
        try
        {
            fontsize = std::stoi(fontsizes);
        } catch (std::invalid_argument& e) {return 2;}
        if (lang == "en") language = 0;
        else if (lang == "de") language = 1;
        else return 2;
        return 0;
    }

    u8 boo::Config::Save(std::string file)
    {
        std::ofstream ini(file);
        ini << "[config]\n";
        ini << "StageData=" << StageDataPath << "\n";
        ini << "ObjectData=" << ObjectDataPath << "\n";
        std::string lang = "en";
        if (language == 1) lang = "de";
        ini << "language=" << lang << "\n";
        ini << "fontsize=" << std::to_string(fontsize) << "";
        ini.close();
        if (!ini) return 1;
        return 0;
    }

    Config& boo::Config::Get()
    {
        static Config config;
        return config;
    }

}