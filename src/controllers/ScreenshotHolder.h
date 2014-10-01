#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"

class ScreenshotHolder
{
	public:		
		static ci::Surface screenshot;
		static ci::Surface screenshotSmall;
		static void resize();
};