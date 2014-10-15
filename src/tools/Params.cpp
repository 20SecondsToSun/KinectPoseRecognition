#include "Params.h"

float	Params::boxMaxErrorX = 350;
float	Params::boxMaxErrorY= 350;
float	Params::maxErrorBetweenJoints = 40;
float	Params::percentForMatching = 0.9f;
float 	Params::comeBackHomeTime = 20;
bool	Params::isNetConnected = true;
std::string	Params::standID = "-1";

fs::path		Params::getPhotosStorageDirectory()
{
	return getAppPath()/"Documents"/"Canon Pictures";
}

fs::path		Params::getTempStorageDirectory()
{
	return getAppPath()/"data"/"temp";
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