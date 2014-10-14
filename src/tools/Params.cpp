#include "Params.h"

float	Params::boxMaxErrorX = 350;
float	Params::boxMaxErrorY= 350;
float	Params::maxErrorBetweenJoints = 40;
float	Params::percentForMatching = 0.9f;
float 	Params::comeBackHomeTime = 20;
bool	Params::isNetConnected = true;

fs::path		Params::getPhotosStorageDirectory()
{
	return getAppPath()/"Documents"/"Canon Pictures";
}

fs::path		Params::getTempStorageDirectory()
{
	return getAppPath()/"temp";
}

fs::path		Params::getFinalImageStoragePath()
{
	return getAppPath()/"temp"/ fs::path( "finalimage.jpg");
}

