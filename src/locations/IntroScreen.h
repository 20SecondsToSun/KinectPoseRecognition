#pragma once

#include "Params.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "ButtonColor.h"
#include "MainGameScreen.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

class IntroScreen : public Location
{
	public:
		void setup();
		void init( LocationEngine* game);
		void cleanup();

		void pause(){};
		void resume(){};

		void handleEvents(){};
		void mouseEvents();
		void update();
		void keyEvents();
		void draw();

		static IntroScreen* Instance() {
			return &IntroScreenState;
		}

		void	gotoFirstScreen();

	private:	

		enum states {INIT, 
					 SHOW_INVITE,  
					 SHOW_INSTRUCTION,
					 START_GAME,
					 ANIMATE_TO_SHOW_INVITE};

		LocationEngine*			_game;
		static IntroScreen		IntroScreenState;	

		ci::signals::connection startInstructionBtnSignal;
		ci::signals::connection startGameBtnSignal;
		ci::signals::connection comeBackBtnSignal;


		int						state, nextState;
		std::string				debugString;		
	
		ci::gl::Texture			cat, logo, text1, playImage, instructionImage;
	
		ButtonColor				*startInstructionBtn, *startGameBtn, *comeBackBtn;

		ci::Anim<float>			alphaAnimate, catAnimate, logoAnimate,textAnimateY, textAnimateAlpha;	

		void	startInstructionBtnDown();
		void	startGameBtnDown();
		void	drawInitElements();
		void	animationFinished();
		void	changeState();
};