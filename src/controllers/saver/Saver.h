#pragma once

#include "Params.h"
#include "Pose.h"
#include "cinder/Json.h"
#include <map>

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
		void loadConfigData();
		std::vector<Pose*> Saver::loadPoseBase();

		bool	saveImageIntoBase(std::string mails,  ci::Surface  image);
};

inline Saver&	saver() { return Saver::getInstance(); };