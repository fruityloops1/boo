#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <oead/types.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace boo
{

class Localization
{
private:
    static const inline std::vector<std::unordered_map<std::string, std::string>> data = {
        { // English
            {"title", "Boo Editor"}
        },
        { // German
            {"title", "Boo Editor"}
        }
    };
    static inline u8 language;
public:
    static bool SetLanguage(u8 lang)
    {
        if (lang >= 2)
        {
            return false;
        }
        boo::Localization::language = lang;
        return true;
    }
    static std::string GetLocalized(std::string identifier)
    {
        try
        {
            return data[language].at(identifier);
        } catch (std::out_of_range& e) {return "NULL";}
    }
};

}

#endif