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

using namespace ci;
using namespace ci::gl;
class PhotoMaker
{
	typedef boost::signals2::signal<void(void )> photoCreateSignal;	

private:
	Fbo	 mFbo, cropPhotoFbo;
	Timer	dirUploadTimer;

	void drawCropPhotoToFBO(Vec2f poseShiftVec, Surface comicsTexture);
	void drawAllInFinallFBO(Surface img, Texture comicsImage, bool isMask);
	
	Texture readyMaskedTexture;
	float mainScale;
	void clearFBO(Fbo fbo);

	ci::gl::Texture getValidTexture(int index);

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