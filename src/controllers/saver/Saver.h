#pragma once

#include "Params.h"
#include "Pose.h"
#include "cinder/Json.h"
#include <map>

namespace SaverDefaults
{
	static const std::string  DATA_BASE_NAME		= "poses.json";
	static const std::string  JSON_STORAGE_FOLDER	= "poses";
}

class Saver
{
	private:
		Saver() {};
		~Saver() {};
		bool	checkFile(fs::path filepath, std::string mails);
		int		getImagesInDir(fs::path dir_path);
		
	public:
		// singleton implementation
		static Saver& getInstance() { 
			static Saver saver; 
			return saver; 
		};

		void savePoseIntoBase(Pose* pose);
		std::vector<Pose*> Saver::loadPoseBase();

		bool	saveImageIntoBase(std::string mails,  ci::Surface  image);

};

// helper function(s) for easier access 
inline Saver&	saver() { return Saver::getInstance(); };