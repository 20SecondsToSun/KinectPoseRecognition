#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
//#include "cinder/Text.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "Utils.h"
#include "FontStore.h"
#include "ButtonColor.h"
#include "AssetsManager.h"
#include "KinectAdapter.h"
#include "MainGameScreen.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace instructionDefaults
{
	enum states {INIT, 
				 SHOW_INVITE,  
				 SHOW_INSTRUCTION,
				 START_GAME,
				 ANIMATE_TO_SHOW_INVITE};
}

class IntroScreen : public Location
{

public:
	void setup();
	void init( LocationEngine* game);
	void cleanup();

	void pause();
	void resume();

	void handleEvents();
	void mouseEvents();
	void update();
	void keyEvents();
	void draw();

	static IntroScreen* Instance() {
		return &IntroScreenState;
	}

protected:
	IntroScreen() { };

private:	

	LocationEngine*			_game;
	static IntroScreen		IntroScreenState;	

	void					animationFinished();
	ci::Anim<float>			alphaAnimate;
	

	int						state, nextState;
	ci::gl::Texture			introImage, playImage, instructionImage;

	bool					isPeopleInFrame;

	void					changeState();
	bool					isChangingStateNow;


	ci::Timer				returnTimer;

	void					checkReturnTimer();
	void					checkReturnTimer2();

	std::string				debugString;

	ButtonColor				*startInstructionBtn, *startGameBtn;


	void					startInstructionBtnDown();
	void					startGameBtnDown();
	void					drawInitElements() ;
};