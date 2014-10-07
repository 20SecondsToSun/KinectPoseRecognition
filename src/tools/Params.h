#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"
#include "Utils.h"
#include "FontStore.h"
#include "AssetsManager.h"

//#define kinectInUse
//#define   recording
#define debug	

namespace poseParams
{
	const float	  BOX_SCALE							= 250.f;
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

};