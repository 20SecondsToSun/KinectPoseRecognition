#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"
#include "Params.h"
#include "AssetsManager.h"

using namespace  poseParams;

class PlayerData
{
	public:
		struct OnePlayerPoseData
		{
			ci::Surface screenshot;
			bool isSuccess;
			bool isMask;
			int  storyCode;
			std::string pathHiRes;
			bool	 isFocusError;
			ci::gl::Texture imageTexture;
			ci::ImageSourceRef imageSourceRef;	
		};

		struct FragmentOptions
		{
			int fboWidth;
			int fboHeight;
			float poseScale;
			ci::Vec2f poseShiftVec;
			gl::Texture mask;
		};
		static FragmentOptions fragmentOptions;

		static OnePlayerPoseData playerData[POSE_IN_GAME_TOTAL];
		static int score;
		static int photosWithoutError;

		static void initData();	

		static std::string getTexPath(int i);
	
		static ci::Surface finalImageSurface;

		static void setFragmentScaleOptions(int fboWidth, int fboHeight, float poseScale, ci::Vec2f poseShiftVec);

		static int getFragmentWidth();
		static int getFragmentHeight();
		static ci::Vec2f getFragmentShiftVec();	
		
		static ci::Vec2f finalShift;	
};