#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include <map>
#include "Params.h"
#include "PlayerData.h"
#include "cinder/gl/Fbo.h"
#include "CameraAdapter.h"
#include "TextureManager.h"
#include "Game.h"
#include "cinder/gl/GlslProg.h"

class PhotoMaker
{
	typedef boost::signals2::signal<void(void )> photoCreateSignal;	

private:
	ci::gl::Fbo	 mFbo, mFbo1, mFboFirst, mFboPoseMaskTexture1;
	ci::Timer	dirUploadTimer;
	void drawToFBO(ci::Surface img, ci::gl::Texture comicsImage);
	void drawToFBOFragmentScale(ci::Vec2f poseShiftVec, ci::Surface comicsTexture);
	void drawToFBOFirstCadr(ci::Surface comicsTexture);
	void drawToFBOposeMaskTexture1(ci::Surface img);
	gl::GlslProg maskShader;
	gl::Texture finalMask;
public:
	static PhotoMaker& getInstance() { static PhotoMaker pht; return pht; };

	void startTimer();
	void stopTimer();
	int  getElapsedSeconds();
	void loadFinalImages();
	bool resizeFinalImages();

	void setup();

	photoCreateSignal photoLoadEvent, photoLoadErrorEvent;
};

inline PhotoMaker&	photoMaker() { return PhotoMaker::getInstance(); };