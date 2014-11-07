#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"
#include "Utils.h"
#include "FontStore.h"

#define kinectUsed
//#define recording
//#define calibration

//#define debug	

//#define alwayswin
//#define paramsDraw	
//#define drawTimer

namespace colorsParams
{
	const ci::Color   RED         =  ci::Color(1.0f, 0.0f, 0.0f);
	const ci::Color   GREEN       =  ci::Color(0.0f, 1.0f, 0.0f);
	const ci::Color   BLUE        =  ci::Color(0.0f, 0.0f, 1.0f);	
	const ci::Color   BLUE_FADE   =  ci::Color(102.0f/255.0f, 206.0f/255.0f, 245.0f/255.0f);
}

namespace photoParams
{
	const int   BIG_PHOTO_WIDTH    = 1000;
	const int   BIG_PHOTO_HEIGHT   = 563;
}

namespace photoMakerParams
{
	const int MAX_WAITING_FROM_DIR_TIME = 7;
}

namespace poseParams
{
	const float				  BOX_SCALE							= 250.f;
	const int				  POSE_IN_GAME_TOTAL				= 3;
	static const std::string  DATA_BASE_NAME					= "poses.json";
	static const std::string  JSON_STORAGE_FOLDER				= "poses";

	const float MIN_DISTANCE_TO_SKELET = 1.3f;
}

namespace popupTypes
{
	static const int  FACEBOOK = 0;
	static const int  VKONTAKTE = 1;
	static const int  EMAIL = 3;
}

namespace serverParams
{
	static const int	 SERVER_WAITING_TIME = 10;
	const std::string    connectionTestURL	 =  "http://google.com";
	const std::string    serverURL			 =  "http://kotopoza.ru/utils/upload/";
	const std::string    mailURL			 =  "http://kotopoza.ru/utils/send2mail";
	const std::string    badTestURL			 =  "http://catpos.familyagency.ru/utils/upload1/";
}

class Params
{
	public:	

		static float	boxMaxErrorX;
		static float	boxMaxErrorY;
		static float	maxErrorBetweenJoints;
		static float	minErrorBetweenJoints;
		static float	percentForMatching;

		static float	weightJoints[12];
	
		static float	comeBackHomeTime;
		static fs::path	getPhotosStorageDirectory();
		static fs::path	getTempPhotoSavePath(int i);
		static fs::path	getTempStorageDirectory();		
		static fs::path	getFinalImageStoragePath();
		static fs::path	getConfigStoragePath();
		static fs::path	getBaseStoragePath();	
		static fs::path	getPoseSavingPath();

		static bool		isNetConnected;
		static bool		photoFromDirError;

		static bool		serverPhotoLoadError;
		static bool		serverPhotoLoadTimeout;

		static bool		serverConnectionCheckError;
		static bool		serverConnectionCheckTimeout;

		static bool		serverEmailSendError;
		static bool		serverEmailSendTimeout;
		static bool		isFullSkelet;
		static bool		isPointsDraw;
		
		static int		computeMistakeAlgo;

		static int		standID;
};