#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
//#include "cinder/Text.h"
#include "cinder/Timer.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"


#include "PlayerData.h"
#include "Location.h"
#include "Utils.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace ReadyScreenDefaults
{
	//nst int  FONS_SCREENS_NUM					    = 6;
	//um states { FLASH, SCREESHOT_ANIM, LOAD_TO_SERVER, BLOCK };
}
class ResultScreen : public Location
{

public:
	void setup();
	void init( LocationEngine* game);
	void cleanup();

	void pause();
	void resume();

	void shutdown();

	void handleEvents();
	void mouseEvents();
	void keyEvents();
	void update();
	void draw();

	static ResultScreen* Instance() {
		return &ResultScreenState;
	}

protected:
	ResultScreen() { }

private:
	
	void							animationFlashFinish();
	void							animationLastFinish();
	void							animationOutFinish();

	LocationEngine*					_game;
	static ResultScreen				ResultScreenState;
	
	Texture*						logoTexture, *helloTexture;	
	ci::Anim<float>					alphaFade;
	std::vector< ci::gl::Texture*>  fonImgVector;
	int								currentFon;

	ci::Surface						screnshot;

	//ReadyScreenDefaults::states		state;

	ci::Anim<float>					alphaFlash;
	ci::Anim<ci::Vec2f>				startPhotoScale, startPhotoXY;

	ci::Font						hintFont;

	float							oneWidth;
};