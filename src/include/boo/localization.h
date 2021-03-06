#pragma once

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
            {"generating", "Generating Object Parameter Database..."}, {"generating_info", "This could take a while."},
            {"save", "Save"}, {"language", "Language"}, {"fontsize", "GUI Scale"}, {"restart_changes", "Restart Boo Editor for the changes to take effect."},
            {"file", "File"}, {"exit", "Exit"}, {"exit_shortcut", "CTRL Q"}, {"stages", "Stages"}, {"open", "Open"}, {"open_shortcut", "CTRL O"},
            {"t_sarc", "SARC archive"}, {"t_szs", "Yaz0-compressed SARC archive"}, {"scenario", "Scenario"}, {"object", "Object"},
            {"no_object", "There are no objects selected."}, {"multiple_objects", "Multiple objects selected."},
            {"position", "Position"}, {"rotation", "Rotation"}, {"scale", "Scale"}, {"save", "Save"}, {"save_shortcut", "CTRL S"}, {"save_as", "Save As..."}, {"save_as_shortcut", "CTRL SHIFT S"},
            {"no_stages", "No stages opened."}, {"save_sure", "The file has unchanged changes. Are you sure?"}, {"yes", "Yes"}, {"no", "No"},
            {"close_stage", "Close stage"}, {"param_Id", "Object ID"}, {"param_LayerConfigName", "Layer"}, {"param_ModelName", "Model Name"}, 
            {"param_UnitConfigName", "Object Name"}, {"param_ParameterConfigName", "Class Name"}, {"param_DisplayName", "Display Name"}, {"param_comment", "Comment"}, {"filter", "Filter"},
            {"loading", "Loading"}, {"saving", "Saving"}, {"s_randomizer", "Randomizer"}, {"extras", "Extras"}, {"output_folder", "Output folder"},
            {"randomize_shines", "Randomize moons"}, {"randomize_scenarios", "Randomize Kingdom loading zone scenarios"}, {"preparing", "Preparing"},
            {"randomizer_notice", "This is a Randomizer that will shuffle up all loading zones.\nIt can also randomize moons, music, and the scenarios after entering a Kingdom loading zone."},
            {"properties", "Properties"}, {"links", "Links"}, {"return", "Return"}, {"randomize_bgm", "Randomize music"},
            {"object_already_exists", "An object with that ID already exists!"}, {"loaded_s", "Loaded "}, {"closed_s", "Closed "}, {"randomizer_done", "Done randomizing"},
            {"saved_s", "Saved "}
        },
        { // German
            {"title", "Boo Editor"}, {"debug", "Debug"}, {"view", "Ansicht"}, {"object_view", "Objektansicht"},
            {"s_debug", "Debug-Men??"}, {"s_object_view", "Objektansicht"}, {"s_preferences", "Einstellungen"}, {"select_stage_data", "StageData Ordner ausw??hlen"},
            {"stage_data_notice", "Um Boo Editor zu nutzen, ben??tigst du den StageData Ordner deines RomFS-Dumps."},
            {"browse", "Durchsuchen"}, {"stage_data_folder", "StageData Ordner"}, {"object_data_folder", "ObjectData Ordner"}, {"not_exist", "Der Ordner existiert nicht"}, {"ok", "OK"},
            {"generate_opdb", "Objektparameterdatenbank"}, {"generate", "Generieren"},
            {"generate_notice", "Um Boo Editor zu nutzen, ben??tigst du eine Objektparameterdatenbank. Diese kann durch dem StageData Ordner generiert werden."},
            {"generating", "Generiere Objektparameterdatenbank..."}, {"generating_info", "Dies k??nnte etwas dauern."},
            {"save", "Speichern"}, {"language", "Sprache"}, {"fontsize", "GUI-Skalierung"}, {"restart_changes", "Starte Boo Editor neu um die ??nderungen wirksam zu machen."},
            {"file", "Datei"}, {"exit", "Schlie??en"}, {"exit_shortcut", "STRG-Q"}, {"stages", "Level"}, {"open", "??ffnen"}, {"open_shortcut", "STRG-O"},
            {"t_sarc", "SARC-Archiv"}, {"t_szs", "Yaz0-komprimiertes SARC-Archiv"}, {"scenario", "Szenario"}, {"object", "Objekt"},
            {"no_object", "Es sind keine Objekte ausgew??hlt."}, {"multiple_objects", "Mehrere Objekte ausgew??hlt."},
            {"position", "Position"}, {"rotation", "Drehung"}, {"scale", "Skalierung"}, {"save", "Speichern"}, {"save_shortcut", "STRG-S"}, {"save_as", "Speichern Als..."}, {"save_as_shortcut", "STRG-SHIFT-S"},
            {"no_stages", "Keine Level ge??ffnet."}, {"save_sure", "Die Datei hat ungespeicherte ??nderungen. Fortfahren?"}, {"yes", "Ja"}, {"no", "Nein"},
            {"close_stage", "Level schlie??en"}, {"param_Id", "Objekt-ID"}, {"param_LayerConfigName", "Ebene"}, {"param_ModelName", "Modelname"}, 
            {"param_UnitConfigName", "Objektname"}, {"param_ParameterConfigName", "Klassenname"}, {"param_DisplayName", "Anzeigename"}, {"param_comment", "Kommentar"}, {"filter", "Filter"},
            {"loading", "Lade"}, {"saving", "Speichere"}, {"s_randomizer", "Randomizer"}, {"randomizer", "Randomizer"}, {"extras", "Extras"}, {"output_folder", "Ausgabeordner"},
            {"randomize_shines", "Monde mischen"}, {"randomize_scenarios", "K??nigreichsladezonenszenarien mischen"}, {"preparing", "Bereite vor"},
            {"randomizer_notice", "Dies ist ein Randomizer der alle Ladezonen vermischen wird.\nEr kann auch Monde, Musik, und K??nigreichsladezonenszenarien vermischen."},
            {"properties", "Eigenschaften"}, {"links", "Verbindungen"}, {"return", "Zur??ck"}, {"randomize_bgm", "Zuf??llige Musik"},
            {"object_already_exists", "Ein Objekt mit dieser ID existiert schon!"}, {"loaded_s", "Ladete "}, {"closed_s", "Schlo?? "}, {"randomizer_done", "Randomizer abgeschlossen"},
            {"saved_s", "Speicherte "}
        }
    };
    static inline u8 language;
public:
    static bool setLanguage(u8 lang)
    {
        if (lang >= 2)
        {
            return false;
        }
        boo::Localization::language = lang;
        return true;
    }
    static inline std::string getLocalized(std::string identifier)
    {
        try
        {
            return data[language].at(identifier);
        } catch (std::out_of_range& e) {return "NULL";}
    }
};

}