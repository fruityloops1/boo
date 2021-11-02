#include <boo/config.h>
#include <boo/localization.h>
#include <boo/randomizer.h>
#include <boo/stage.h>

#include <filesystem>
#include <fstream>
#include <random>

namespace boo
{
    void Randomizer::RandomizeLZ(bool shines, bool scenarios, std::string path, std::string output, std::string* progress)
    {
	    std::vector<boo::Stage> stages_data;

		std::string stagedata_path = output;
		stagedata_path.append("/StageData/");

		if (!std::filesystem::exists(stagedata_path)) std::filesystem::create_directories(stagedata_path);

	    std::unordered_map<std::string, std::vector<std::string>> stages; // List of Stages and their entry positions

		if (progress != nullptr)
        {
            *progress = boo::Localization::GetLocalized("preparing");
        }

		int max_stage = 1;
		for (const auto& sf : std::filesystem::directory_iterator(path))
		{
			if (sf.path().filename().string().starts_with("Revenge")) continue;
			if (!sf.path().filename().string().ends_with("Map.szs")) continue;
		    if (sf.path().filename().string().find("Demo") != std::string::npos) continue;
			max_stage++;
		}

		int current_stage = 0;
        for (const auto& sf : std::filesystem::directory_iterator(path))
        {
			if (sf.path().filename().string().starts_with("Revenge")) continue;
		    if (!sf.path().filename().string().ends_with("Map.szs")) continue;
		    if (sf.path().filename().string().find("Demo") != std::string::npos) continue;
            stages_data.push_back(boo::Stage());
		    boo::Stage& stage = stages_data[stages_data.size() - 1];

		    std::ifstream opf(sf.path(), std::ios::binary);
    	    std::istreambuf_iterator<char> start(opf), end;
    	    std::vector<u8> sarc(start, end);
    	    opf.close();

			current_stage++;

            if (progress != nullptr)
            {
                *progress = boo::Localization::GetLocalized("loading");
                progress->append(" ");
                progress->append(sf.path().filename());
                progress->append(" (");
				progress->append(std::to_string(current_stage));
                progress->append("/");
				progress->append(std::to_string(max_stage));
                progress->append(")");
            }

		    stage.Load(sarc);

		    for (boo::StageDataEntry& sde : stage.data.entries)
		    {
			for (auto ol = sde.object_lists.begin(); ol != sde.object_lists.end(); ++ol)
			    {
				    for (boo::Object& object : ol->second.objects)
				    {
					    if (object.extra_params.find("ChangeStageId") != object.extra_params.end() &&
						    object.extra_params.find("ChangeStageName") != object.extra_params.end())
					    {
						    if (object.extra_params["ChangeStageId"].GetType() == oead::Byml::Type::String &&
							    object.extra_params["ChangeStageName"].GetType() == oead::Byml::Type::String)
							    stages[object.extra_params["ChangeStageName"].GetString()].push_back(object.extra_params["ChangeStageId"].GetString());
					    }
				    }
			    }
		    }
        }

	    std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> mt(0, stages.size() - 1);

		current_stage = 0;

	    for (boo::Stage& stage : stages_data)
	    {

			current_stage++;

            if (progress != nullptr)
            {
                *progress = boo::Localization::GetLocalized("saving");
                progress->append(" ");
                progress->append(stage.Name);
                progress->append(" (");
				progress->append(std::to_string(current_stage));
                progress->append("/");
				progress->append(std::to_string(max_stage));
                progress->append(")");
            }

		    for (boo::StageDataEntry& sde : stage.data.entries)
		    {
			    for (auto ol = sde.object_lists.begin(); ol != sde.object_lists.end(); ++ol)
			    {
			    	for (boo::Object& object : ol->second.objects)
		    		{
	    				if (object.UnitConfig.ParameterConfigName.find("Shine") != std::string::npos) continue;
    					if (object.extra_params.find("ChangeStageId") != object.extra_params.end() &&
					    	object.extra_params.find("ChangeStageName") != object.extra_params.end())
				    	{
			    			object.extra_params.erase("ChangeStageId");
		    				object.extra_params.erase("ChangeStageName");
	    					object.extra_params.erase("ScenarioNo");

			    			auto it = stages.begin();
		    				std::advance(it, mt(rng) % stages.size());
	    					std::string k = it->first;

			    			object.extra_params["ChangeStageName"] = oead::Byml(k);
    	    				std::uniform_int_distribution<std::mt19937::result_type> mtt(0, stages[k].size() - 1);
	    					object.extra_params["ChangeStageId"] = oead::Byml(stages[k][mtt(rng)]);

		    				if (scenarios && k.ends_with("HomeStage"))
	    					{
    							std::uniform_int_distribution<std::mt19937::result_type> smt;
							    if (k == "CapWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
						    	else if (k == "WaterfallWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
					    		else if (k == "SandWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 4);
				    			else if (k == "ForestWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 4);
			    				else if (k == "LakeWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
		    					else if (k == "CloudWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
	    						else if (k == "ClashWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
    							else if (k == "CityWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 9);
					    		else if (k == "SeaWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
				    			else if (k == "SnowWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
			    				else if (k == "LavaWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 9);
		    					else if (k == "BossRaidWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
	    						else if (k == "SkyWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 3);
    							else if (k == "MoonWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 2);
							    else if (k == "PeachWorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 2);
							    else if (k == "Special1WorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 1);
							    else if (k == "Special2WorldHomeStage") smt = std::uniform_int_distribution<std::mt19937::result_type>(0, 1);
							    object.extra_params["ScenarioNo"] = oead::Byml(oead::Number<int>(smt(rng)));
						    }
						    else object.extra_params["ScenarioNo"] = oead::Byml(oead::Number<int>(-1));
					    }
				    }
			    }
		    }

		    std::vector<u8> file;
    	    file = stage.Save(true);
		    std::string opath = std::string(stagedata_path);
		    opath.append(stage.Name);
		    opath.append("Map.szs");
    	    std::ofstream stagefile2(opath, std::ios::out | std::ios::binary);
    	    stagefile2.write(reinterpret_cast<const char*>(&file[0]), file.size()*sizeof(u8));
    	    stagefile2.close();
	    }

		if (progress != nullptr)
		{
			*progress = "Done";
		}

    }
}