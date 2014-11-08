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

class PhotoMaker
{
	typedef boost::signals2::signal<void(void )> photoCreateSignal;	

private:
	ci::gl::Fbo	 mFbo;
	ci::Timer	dirUploadTimer;
	void drawToFBO(ci::Surface img, ci::gl::Texture comicsImage);

public:
	static PhotoMaker& getInstance() { static PhotoMaker pht; return pht; };

	void startTimer();
	void stopTimer();
	int  getElapsedSeconds();
	void loadFinalImages();
	bool resizeFinalImages();

	photoCreateSignal photoLoadEvent, photoLoadErrorEvent;
};

inline PhotoMaker&	photoMaker() { return PhotoMaker::getInstance(); };