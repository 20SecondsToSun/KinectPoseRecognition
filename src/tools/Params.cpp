#include "Params.h"

float	Params::boxMaxErrorX = 350;
float	Params::boxMaxErrorY= 350;
float	Params::maxErrorBetweenJoints = 40;
float	Params::percentForMatching = 0.9f;
float 	Params::comeBackHomeTime = 20;

fs::path		Params::getPhotosStorageDirectory()
{
	return getAppPath()/"Documents"/"Canon Pictures";
}