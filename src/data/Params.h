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
#define testServer	

#define alwayswin
//#define paramsDraw	
//#define drawTimer
//#define recording

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
	const float				  BOX_SCALE							= 240.f;
	const int				  POSE_IN_GAME_TOTAL				= 1;
	static const std::string  DATA_BASE_NAME					= "poses.json";
	static const std::string  JSON_STORAGE_FOLDER				= "poses";
}

namespace popupTypes
{
	static const int  FACEBOOK = 0;
	static const int  VKONTAKTE = 1;
	static const int  EMAIL = 3;
}

class Params
{
public:	

	static float	boxMaxErrorX;
	static float	boxMaxErrorY;
	static float	maxErrorBetweenJoints;
	static float	minErrorBetweenJoints;
	static float	percentForMatching;

	static float	maxUserDistance;
	static float	minUserDistance;
	static float	maxUserHeight;
	static float	minUserHeight;

	static float	weightJoints[12];

	static float	comeBackHomeTime;
	static fs::path	getPhotosStorageDirectory();
	static fs::path	getTempPhotoSavePath(int i);
	static fs::path	getTempStorageDirectory();
	static fs::path	getFinalImageStoragePath();
	static fs::path	getConfigStoragePath();
	static fs::path	getBaseStoragePath();
	static fs::path	getPoseSavingPath();

	static void		saveBufferSuccessComics(ci::gl::Texture);
	static ci::gl::Texture getBufferSuccessComics();

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

	static float	etalonHeight;

	static ci::gl::Texture _successComics;
};