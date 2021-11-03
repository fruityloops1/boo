#include <boo/config.h>
#include <boo/localization.h>
#include <boo/randomizer.h>
#include <boo/stage.h>

#include <oead/yaz0.h>

#include <filesystem>
#include <fstream>
#include <random>

namespace boo
{
    void Randomizer::RandomizeLZ(bool shines, bool scenarios, std::string path, std::string output, std::string* progress)
    {
	    std::vector<boo::Stage> stages_data;

		std::string stagedata_path = std::string(output);
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

	struct ShineInfo
	{
		int HintIdx;
		bool IsAchievement;
		bool IsGrand;
		bool IsMoonRock;
		int MainScenarioNo;
		std::string ObjId;
		std::string ObjectName;
		int ProgressBitFlag;
		std::string ScenarioName;
		std::string StageName;
		oead::Vector3f Trans;
		int UniqueId;
	};

	void Randomizer::RandomizeShines(std::string path, std::string output)
	{
		std::string systemdata_path = std::string(output);
		systemdata_path.append("/SystemData/");

		if (!std::filesystem::exists(systemdata_path)) std::filesystem::create_directories(systemdata_path);

		std::ifstream opf(path, std::ios::binary);
    	std::istreambuf_iterator<char> start(opf), end;
    	std::vector<u8> sarc(start, end);
    	opf.close();

		sarc = oead::yaz0::Decompress(sarc);

		std::map<std::string, int> shine_lists;
		std::vector<ShineInfo> sifos;
		std::vector<std::tuple<std::string, std::string, int>> sifos_r;

		std::vector<u8> PosList;

		oead::Sarc si = oead::Sarc(sarc);
		for (auto f : si.GetFiles())
		{
			if (f.name.starts_with("ShineList_"))
			{
				std::string stage = std::string(f.name);
				stage.erase(stage.length() - 5);
				stage.erase(0, 10);

				oead::Byml root = oead::Byml::FromBinary(f.data);
				int count = 0;
				for (oead::Byml& sle : root.GetHash().at("ShineList").GetArray())
				{
					ShineInfo sie;
					sie.HintIdx = sle.GetHash().at("HintIdx").GetInt();
					sie.IsAchievement = sle.GetHash().at("IsAchievement").GetBool();
					sie.IsGrand = sle.GetHash().at("IsGrand").GetBool();
					sie.IsMoonRock = sle.GetHash().at("IsMoonRock").GetBool();
					sie.MainScenarioNo = sle.GetHash().at("MainScenarioNo").GetInt();
					sie.ObjId = sle.GetHash().at("ObjId").GetString();
					sie.ObjectName = sle.GetHash().at("ObjectName").GetString();
					sie.ProgressBitFlag = sle.GetHash().at("ProgressBitFlag").GetInt();
					sie.ScenarioName = sle.GetHash().at("ScenarioName").GetString();
					sie.StageName = sle.GetHash().at("StageName").GetString();

					sie.Trans.x = sle.GetHash().at("Trans").GetHash().at("X").GetFloat();
					sie.Trans.y = sle.GetHash().at("Trans").GetHash().at("Y").GetFloat();
					sie.Trans.z = sle.GetHash().at("Trans").GetHash().at("Z").GetFloat();

					sie.UniqueId = sle.GetHash().at("UniqueId").GetInt();
					sifos.push_back(sie);

					sifos_r.push_back(std::tuple<std::string, std::string, int>(sie.StageName, sie.ObjId, sie.UniqueId));

					count++;
				}
				shine_lists[stage] = count;
			}
		}

		std::srand(std::time(nullptr));
		std::random_shuffle(sifos_r.begin(), sifos_r.end());

		int gc = 0;

		for (auto sl = shine_lists.begin(); sl != shine_lists.end(); ++sl)
		{
			for (int i = 0; i < sl->second; i++)
			{
				sifos[gc].StageName = std::get<0>(sifos_r[gc]);
				sifos[gc].ObjId = std::get<1>(sifos_r[gc]);
				sifos[gc].UniqueId = gc;
				gc++;
			}
		}

		oead::SarcWriter sw;
		sw.m_files["ShinePosList.byml"] = std::vector<u8>();
		sw.m_files["ShinePosList.byml"].assign(si.GetFile("ShinePosList.byml")->data.begin(), si.GetFile("ShinePosList.byml")->data.end());
		gc = 0;
	
		for (auto sl = shine_lists.begin(); sl != shine_lists.end(); ++sl)
		{
			oead::Byml root = oead::Byml(absl::btree_map<std::basic_string<char>, oead::Byml>());
			root.GetHash()["ShineList"] = oead::Byml(std::vector<oead::Byml>());
			for (int i = 0; i < sl->second; i++)
			{
				root.GetHash()["ShineList"].GetArray().push_back(oead::Byml(absl::btree_map<std::basic_string<char>, oead::Byml>()));
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["HintIdx"] = oead::Byml(oead::Number<int>(sifos[gc].HintIdx));
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["IsAchievement"] = oead::Byml(sifos[gc].IsAchievement);
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["IsGrand"] = oead::Byml(sifos[gc].IsGrand);
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["IsMoonRock"] = oead::Byml(sifos[gc].IsMoonRock);
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["MainScenarioNo"] = oead::Byml(oead::Number<int>(sifos[gc].MainScenarioNo));
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["ObjId"] = oead::Byml(sifos[gc].ObjId);
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["ObjectName"] = oead::Byml(sifos[gc].ObjectName);
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["ProgressBitFlag"] = oead::Byml(oead::Number<int>(sifos[gc].ProgressBitFlag));
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["ScenarioName"] = oead::Byml(sifos[gc].ScenarioName);
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["StageName"] = oead::Byml(sifos[gc].StageName);

				root.GetHash()["ShineList"].GetArray()[i].GetHash()["Trans"] = oead::Byml(absl::btree_map<std::basic_string<char>, oead::Byml>());
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["Trans"].GetHash()["X"] = oead::Byml(oead::Number<float>(sifos[gc].Trans.x));
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["Trans"].GetHash()["Y"] = oead::Byml(oead::Number<float>(sifos[gc].Trans.y));
				root.GetHash()["ShineList"].GetArray()[i].GetHash()["Trans"].GetHash()["Z"] = oead::Byml(oead::Number<float>(sifos[gc].Trans.z));

				root.GetHash()["ShineList"].GetArray()[i].GetHash()["UniqueId"] = oead::Byml(oead::Number<int>(sifos[gc].UniqueId));
				gc++;
			}
			std::string fff = "ShineList_";
			fff.append(sl->first);
			fff.append(".byml");
			sw.m_files[fff] = root.ToBinary(false, 3);
		}

		std::vector<u8> result = sw.Write().second;
    	result = oead::yaz0::Compress(result, 0, 9);

		std::string opath = std::string(systemdata_path);
		opath.append("ShineInfo.szs");

    	std::ofstream stagefile2(opath, std::ios::out | std::ios::binary);
    	stagefile2.write(reinterpret_cast<const char*>(&result[0]), result.size()*sizeof(u8));
    	stagefile2.close();

	}
}