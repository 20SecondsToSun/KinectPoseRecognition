#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
//#include "cinder/Text.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "Utils.h"
#include "FontStore.h"
#include "Button.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace instructionDefaults
{
	enum states {INIT, 
				 SHOW_INVITE,  
				 SHOW_INSTRUCTION,
				 START_GAME };
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

	

	void					animationInitFinish() ;
	void					animationFadeOutFinish() ;
	void					animationFinish();
	ci::Anim<float>			alphaFade;




	int						state, nextState;
	ci::gl::Texture			introImage, playImage, instructionImage;

	bool					isPeopleInFrame;

	void					changeState();
	bool					isChangingStateNow;


	ci::Timer				returnTimer;

	void					checkReturnTimer();
	void					checkReturnTimer2();

	std::string				debugString;

	Button					startInstructionBtn, startGameBtn;


	void					startInstructionBtnDown();
	void					startGameBtnDown();
	
};