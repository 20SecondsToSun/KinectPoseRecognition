#pragma once

#include "Params.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "ButtonColor.h"
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

	void	gotoFirstScreen();

protected:
	IntroScreen() { };

private:	

	LocationEngine*			_game;
	static IntroScreen		IntroScreenState;	

	int						state, nextState;
	std::string				debugString;		
	bool					isChangingStateNow;	

	ci::gl::Texture			introImage, playImage, instructionImage;
	
	ButtonColor				*startInstructionBtn, *startGameBtn;

	ci::Anim<float>			alphaAnimate;	

	void					startInstructionBtnDown();
	void					startGameBtnDown();
	void					drawInitElements();
	void					animationFinished();
	void					changeState();

};