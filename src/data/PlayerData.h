#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"
#include "Params.h"

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
			ci::ImageSourceRef imageSourceRef;	
			ci::Vec2f translation;
			float degree;
		};

		static OnePlayerPoseData playerData[POSE_IN_GAME_TOTAL];
		static int	score;

		static void initData();

		static std::string getTexPath(int i);		
		static ci::gl::Texture getDisplayingTexture(int i );
		static ci::Surface finalImageSurface;	
		static void setDisplayingTexture(int, gl::Texture);

		static void setTranslation(int i, ci::Vec2f vec);
		static ci::Vec2f getTranslation(int i);

		static void  setRotation(int i, float _degree);
		static float getRotation(int i);

	private :
		static ci::Vec2f translation;
		static float degree;
};