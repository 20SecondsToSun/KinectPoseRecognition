#pragma once

#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"


#include "ButtonColor.h"
#include "PlayerData.h"
#include "Location.h"
#include "Params.h"
#include "IntroScreen.h"




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

	void gotoFirstScreen();

protected:
	ResultScreen() { }

private:
	LocationEngine*					_game;
	static ResultScreen				ResultScreenState;

	ButtonColor				*mailBtn, *facebookBtn, *vkontakteBtn;

	
	void							animationFlashFinish();
	void							animationLastFinish();
	void							animationOutFinish();

	




	







	
	Texture*						logoTexture, *helloTexture;	
	ci::Anim<float>					alphaFade;
	std::vector< ci::gl::Texture*>  fonImgVector;
	int								currentFon;

	ci::Surface						screnshot;

	//ReadyScreenDefaults::states		state;

	ci::Anim<float>					alphaFlash;
	ci::Anim<ci::Vec2f>				startPhotoScale, startPhotoXY;



	float							oneWidth;

	void					changeState();
	bool					isChangingStateNow;	

	void					animationFinished();
	ci::Anim<float>			alphaAnimate;
};