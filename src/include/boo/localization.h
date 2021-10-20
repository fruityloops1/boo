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
            {"title", "Boo Editor"}, {"debug", "Debug"}, {"view", "View"}, {"object_view", "Object view"},
            {"s_debug", "Debug menu"}, {"s_object_view", "Object view"}, {"s_preferences", "Preferences"}, {"select_stage_data", "Select StageData folder"},
            {"stage_data_notice", "In order to use Boo Editor, you need the StageData folder from your RomFS dump."},
            {"browse", "Browse"}, {"stage_data_folder", "StageData folder"}, {"object_data_folder", "ObjectData folder"}, {"not_exist", "Folder does not exist"}, {"ok", "OK"},
            {"generate_opdb", "Object Parameter Database"}, {"generate", "Generate"},
            {"generate_notice", "In order to use Boo Editor, you need an Object Parameter Database. You can generate it using the StageData folder."},
            {"generating", "Generating Object Parameter Database..."}, {"generating_info", "This could take a bit of time."},
            {"save", "Save"}, {"language", "Language"}, {"fontsize", "GUI Scale"}, {"restart_changes", "Restart Boo Editor for the changes to take effect."},
            {"file", "File"}, {"exit", "Exit"}
        },
        { // German
            {"title", "Boo Editor"}, {"debug", "Debug"}, {"view", "Ansicht"}, {"object_view", "Objektansicht"},
            {"s_debug", "Debug-Menü"}, {"s_object_view", "Objektansicht"}, {"s_preferences", "Einstellungen"}, {"select_stage_data", "StageData Ordner auswählen"},
            {"stage_data_notice", "Um Boo Editor zu nutzen, benötigst du den StageData Ordner deines RomFS-Dumps."},
            {"browse", "Durchsuchen"}, {"stage_data_folder", "StageData Ordner"}, {"object_data_folder", "ObjectData Ordner"}, {"not_exist", "Der Ordner existiert nicht"}, {"ok", "OK"},
            {"generate_opdb", "Objektparameterdatenbank"}, {"generate", "Generieren"},
            {"generate_notice", "Um Boo Editor zu nutzen, benötigst du eine Objektparameterdatenbank. Diese kann durch dem StageData Ordner generiert werden."},
            {"generating", "Generiere Objektparameterdatenbank..."}, {"generating_info", "Dies könnte etwas dauern."},
            {"save", "Speichern"}, {"language", "Sprache"}, {"fontsize", "GUI-Skalierung"}, {"restart_changes", "Starte Boo Editor neu um die Änderungen wirksam zu machen."},
            {"file", "Datei"}, {"exit", "Schließen"}
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