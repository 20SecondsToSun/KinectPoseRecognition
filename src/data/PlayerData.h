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
			int  storyCode;
			std::string pathHiRes;
			bool	 isFocusError;
			ci::gl::Texture imageTexture;
			ci::gl::Texture displayingTexture;
			ci::gl::Texture defaultTexture;
			ci::ImageSourceRef imageSourceRef;	
			ci::Vec2f translation;
			float degree;
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
		static void setup();

		static std::string getTexPath(int i);		
		static ci::gl::Texture getDisplayingTexture(int i );
		static ci::Surface finalImageSurface;	
		static void setDisplayingTexture(int, gl::Texture);

		static void setTranslation(int i, ci::Vec2f vec);
		static ci::Vec2f getTranslation(int i);

		static void  setRotation(int i, float _degree);
		static float getRotation(int i);

		static void  setDefaultTexture(int i, ci::gl::Texture tex);
		static ci::gl::Texture getDefaultTexture(int i);

		static void setFragmentScaleOptions(int fboWidth, int fboHeight, float poseScale, ci::Vec2f poseShiftVec);

		static int getFragmentWidth();
		static int getFragmentHeight();
		static ci::Vec2f getFragmentShiftVec();	

		static gl::Texture kinectTex;
		static gl::Texture poseMaskTexture;

		static ci::Vec2f finalShift;
		static ci::Vec2f finalShift1;

	private :
		static ci::Vec2f translation;
		static float degree;
};