#pragma once

#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "ButtonColor.h"
#include "ButtonTexture.h"
#include "AssetsManager.h"
#include "BubbleAnimator.h"
#include "CatAnimator.h"
#include "MainGameScreen.h"

#include "cinder/gl/Fbo.h"

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
	void mouseEvents(int type);
	void update();
	void keyEvents();
	void draw();

	static IntroScreen* Instance() {
		return &IntroScreenState;
	}

private:
	static const int KINECT_SLEEP_SECONDS = 2; // if we're at the first screen - not to recognize immediately

	enum states {INIT, 
		SHOW_INVITE,  
		SHOW_INSTRUCTION,
		START_GAME,
		ANIMATE_TO_SHOW_INVITE};

	LocationEngine*	 _game;
	static IntroScreen IntroScreenState;	

	ci::signals::connection startInstructionBtnSignal;
	ci::signals::connection startGameBtnSignal;
	ci::signals::connection comeBackBtnSignal;

	int state, nextState;
	string debugString;

	bool deviceError;

	Texture logo, text1, cat2, btnFon, paws,  lapaTv, bg, discl;

	ButtonTex *startInstructionBtn, *startGameBtn, *comeBackBtn;

	ci::Anim<float> alphaAnimate, catAnimate, logoAnimate, textAnimateAlpha, paramBad;	
	ci::Anim<ci::Vec2f> cat2AnimateVec, instructBtnAnimateVec;	
	ci::Anim<ci::Vec2f> lapaTvAnimateVec, startBtnAnimateVec, textAnimateVec;

	void	startInstructionBtnDown();
	void	startGameBtnDown();
	void	drawInitElements();
	void	drawInviteElements();
	void    drawIstructionElements();
	void	animationFinished();
	void	changeState();

	void initInstructionParam();
	void initAnimateParam();
	void inviteAnimateParam();

	void gotoFirstScreen();
	void gotoInviteScreen();

	void (IntroScreen::* drawHandler)();	
};