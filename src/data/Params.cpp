#include "Params.h"

float	Params::boxMaxErrorX = 15.0f;
float	Params::boxMaxErrorY= 15.0f;
float	Params::maxErrorBetweenJoints = 100.0f;
float	Params::minErrorBetweenJoints = 10.0f;

float	Params::percentForMatching = 0.75f;
float 	Params::comeBackHomeTime = 40;
bool	Params::isNetConnected = true;
bool	Params::photoFromDirError = false;

bool	Params::serverPhotoLoadError = false;
bool	Params::serverPhotoLoadTimeout = false;

bool	Params::serverConnectionCheckError = false;
bool	Params::serverConnectionCheckTimeout = false;

bool	Params::serverEmailSendError = false;
bool	Params::serverEmailSendTimeout = false;

int	Params::computeMistakeAlgo = 1;


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
	return getAppPath()/"data"/ fs::path( "data.txt");
}

fs::path		Params::getBaseStoragePath()
{
	return getAppPath()/"data"/poseParams::JSON_STORAGE_FOLDER;
}