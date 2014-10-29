#include "Params.h"

float	Params::boxMaxErrorX = 150;
float	Params::boxMaxErrorY= 150;
float	Params::maxErrorBetweenJoints = 40;
float	Params::percentForMatching = 0.7f;
float 	Params::comeBackHomeTime = 40;
bool	Params::isNetConnected = true;
bool	Params::photoFromDirError = false;

bool	Params::serverPhotoLoadError = false;
bool	Params::serverPhotoLoadTimeout = false;

bool	Params::serverConnectionCheckError = false;
bool	Params::serverConnectionCheckTimeout = false;

bool	Params::serverEmailSendError = false;
bool	Params::serverEmailSendTimeout = false;


std::string	Params::standID = "-1";

fs::path		Params::getPhotosStorageDirectory()
{
	return getAppPath()/"Documents"/"Canon Pictures";
}

fs::path		Params::getTempStorageDirectory()
{
	return getAppPath()/"data"/"temp";
}

fs::path		Params::getTempPhotoSavePath(int i)
{
	return Params::getTempStorageDirectory() / fs::path( "level"+to_string(i+1) +".jpg");	
}

fs::path		Params::getFinalImageStoragePath()
{
	return getAppPath()/"data"/"temp"/ fs::path( "finalimage.jpg");
}

fs::path		Params::getConfigStoragePath()
{
	return getAppPath()/"data"/ fs::path( "data.conf");
}

fs::path		Params::getBaseStoragePath()
{
	return getAppPath()/"data"/poseParams::JSON_STORAGE_FOLDER;
}