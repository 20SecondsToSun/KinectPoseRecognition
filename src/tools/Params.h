#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"
#include "Utils.h"
#include "FontStore.h"
#include "AssetsManager.h"

#define kinectUsed
//#define   recording
#define debug	

namespace poseParams
{
	const float				  BOX_SCALE							= 250.f;
	const int				  POSE_IN_GAME_TOTAL				= 3;
	static const std::string  DATA_BASE_NAME		= "poses.json";
	static const std::string  JSON_STORAGE_FOLDER	= "poses";
}

namespace popupTypes
{
	static const int  FACEBOOK		= 0;
	static const int  VKONTAKTE		= 1;
	static const int  EMAIL			= 3;
}

namespace serverParams
{
	static const int	 SERVER_WAITING_TIME	 = 10;
	const std::string    serverURL			 =  "http://catpos.familyagency.ru/utils/upload/";
	const std::string    mailURL			 =  "http://catpos.familyagency.ru/utils/send2mail";
}

class Params
{
	public:	

		static float	boxMaxErrorX;//50
		static float	boxMaxErrorY;
		static float	maxErrorBetweenJoints;//1000
		static float	percentForMatching;
		//static bool		recording;

		static float	comeBackHomeTime;
		static fs::path	getPhotosStorageDirectory();
		static fs::path	getTempStorageDirectory();		
		static fs::path	getFinalImageStoragePath();
		static fs::path	getConfigStoragePath();
		static fs::path	getBaseStoragePath();
		

		static bool		isNetConnected;
		static std::string	standID;

};