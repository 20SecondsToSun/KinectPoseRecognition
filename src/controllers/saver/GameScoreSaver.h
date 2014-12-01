#pragma once
#include "cinder/app/AppNative.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace ci;
using namespace ci::app;

class GameScoreSaver
{

public:
	static GameScoreSaver& getInstance() { static GameScoreSaver saver; return saver; };

	bool  saveGameStatusIntoFile(bool isWin)
	{
		bool isSave = false;

		fs::path dir_path = getAppPath() / "data\\";
		fs::path file_path = dir_path / ("gameStatus" + to_string(Params::standID) + ".csv");

		if(fs::is_directory(dir_path) || fs::create_directory(dir_path))
		{
			isSave = saveToFile(file_path, getDateByFormat(), isWin);
		}

		return isSave;
	}

private:

	string getDateByFormat()
	{
		time_t     now = time(0);
		struct tm  tstruct;
		tstruct = *localtime(&now);

		string month = to_string(tstruct.tm_mon+1);
		string day   = to_string(tstruct.tm_mday);

		if (month.size() < 2)
			month = "0"+ month;

		if (day.size() < 2)
			day = "0"+ day;

		return to_string(1900 + tstruct.tm_year) + "." + month + "." + day;
	}

	bool saveToFile(fs::path filepath, string dateName, bool isWin)
	{
		const string delimeter = ";";
		const int dayDataLength = 3;

		string line, lastString;
		vector<string> dayGamesStrings;
		ifstream file (filepath.string(), std::ios_base::app);	

		bool pushInTheEnd = false;
		bool status = false;
	
		if (file.is_open())
		{	
			string buffer;	
			while (getline (file, buffer))
				dayGamesStrings.push_back(buffer);

			file.close();

			int32_t stringsSize = dayGamesStrings.size();
			if (stringsSize)
			{
				string lastString = dayGamesStrings[stringsSize-1];
				std::size_t found = lastString.find(dateName);

				if (found != std::string::npos)
				{
					std::vector<std::string> oneDayData;
					boost::split(oneDayData, lastString, boost::is_any_of(delimeter));

					if (oneDayData.size() == dayDataLength)
					{	
						int wingames = atoi(oneDayData[2].c_str());
						int alLGames = atoi(oneDayData[1].c_str());
						dayGamesStrings[stringsSize-1] = generateResultString(oneDayData[0], 
																			  ++alLGames,
																			  isWin ? ++wingames : wingames);

						status = saveAllDayDataInFile(filepath, dayGamesStrings);
					}
				}
				else pushInTheEnd = true;
			}
			else pushInTheEnd = true;	
		}	

		if (pushInTheEnd)
		{
			string resultString  = generateResultString(dateName, 1, isWin ? 1 : 0);
			dayGamesStrings.push_back(resultString);
			status = saveAllDayDataInFile(filepath, dayGamesStrings);
		}

		return status;
	}

	string generateResultString(string data, int allGames, int winGames)
	{	
		return data + ";" + to_string(allGames) + ";" + to_string(winGames);
	}

	bool saveAllDayDataInFile(fs::path filepath, vector<string> dayGamesStrings)
	{
		bool status = false;
		ofstream file (filepath.string());

		if (file.is_open())
		{
			for (int i = 0; i < dayGamesStrings.size(); i++)
				file<<dayGamesStrings[i]<<endl;

			file.close();
			status = true;
		}
	
		return status;
	}
};

inline GameScoreSaver&	gameScoreSaver() { return GameScoreSaver::getInstance(); };