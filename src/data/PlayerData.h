#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"
#include "Params.h"
#include "KinectAdapter.h"

using namespace  poseParams;



class PlayerData
{
	struct OnePlayerPoseData
	{
		ci::Surface screenshot;
		bool isSuccess;
		int  storyCode;
		std::string pathHiRes;
		bool	 isFocusError;
		ci::gl::Texture imageTexture;

		ci::ImageSourceRef imageSourceRef;	
	};

	public:		
		static OnePlayerPoseData playerData[POSE_IN_GAME_TOTAL];
		static int	score;

		static void initData();

		static std::string getTexPath(int i);
		static ci::gl::Texture getComicsImage(int i);
		static ci::Surface finalImageSurface;
	
};