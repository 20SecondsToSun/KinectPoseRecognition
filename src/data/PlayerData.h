#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"

class PlayerData
{
	public:		
		static ci::Surface screenshot[3];
		static bool isSuccess[3];
		static int	score;

		static void initData();
		
};