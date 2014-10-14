#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include <map>
#include "Params.h"
#include "PlayerData.h"
#include "cinder/gl/Fbo.h"
#include "CameraAdapter.h"

namespace photoParams
{
	const int   BIG_PHOTO_WIDTH  = 1000;
	const int   BIG_PHOTO_HEIGHT   = 638;
}

class PhotoMaker
{
	typedef boost::signals2::signal<void(void )> photoCreateSignal;	
private:
	PhotoMaker() {};
	~PhotoMaker() {};

	 std::shared_ptr<std::thread>				loadaImageThread;
	 void loadImagesThreadFn();

public:
	// singleton implementation
	static PhotoMaker& getInstance() { 
		static PhotoMaker fm; 
		return fm; 
	};

	void						loadFinalImages();
	void						resizeFinalImages();
	void						saveFinalImages();

	photoCreateSignal			photoLoadEvent, photoSaveEvent;

	bool						isReady;


	ci::Surface					surface;

	void						drawToFBO(ci::Surface img, ci::gl::Texture comicsImage);

	Fbo					mFbo;

	


};

// helper function(s) for easier access 
inline PhotoMaker&	photoMaker() { return PhotoMaker::getInstance(); };