#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "cinder/Timeline.h"

#include "Location.h"
#include "Utils.h"
#include "IntroScreen.h"
#include "ResultScreen.h"
#include "KinectAdapter.h"
#include "AssetsManager.h"
#include "CameraAdapter.h"

#include "GameControLScreen.h"
#include "HintScreen.h"

using namespace ci;
using namespace ci::app;
using namespace ci::signals;
using namespace gl;
using namespace std;

class MainGameScreen : public Location
{
	public:

		void setup();
		void init( LocationEngine* game);
		void update();
		void draw();
		void cleanup();

		void pause(){};
		void resume(){};
		void handleEvents(){};
		void mouseEvents(int type){};
		void keyEvents(){};		

		static MainGameScreen* Instance()
		{
			return &MainGameScreenState;
		}

	private:

		static const int MAX_MISSED_TIME = 20;

		LocationEngine* _game;
		static MainGameScreen MainGameScreenState;

		void kinectMissPersonHandler();
		void photoFlashHandler();
		void checkPersonMissed();
		bool personisFound();

		bool showGameResultTimeIsFinished();
		void gotoResultScreen();

		void cameraConnectedHandler();
		
		void drawGame();
		void drawPreReadyCounterBox();
		void drawPoseSilhouette();
		
		void drawPoseComics();
		
		void drawCameraLostImage();
		void drawPhotoFlash();
		void drawFadeOutIfAllow();
		void drawDeviceError();

		void animationLeaveLocationPrepare();
		void animationFinished();
		void checkAnimationFinished();
		void animationLeaveLocationFinished();

		void removeTimers();
		void removeHandlers();

		connection comeBackBtnSignal;
		connection cameraConnectionSignal;
		connection kinectConnectionSignal;
		connection kinectMissPersonSignal;
		connection kinectFindPersonSignal;
		connection gotoResultScreenSignal;
		connection photoFlashSignal;

		void gotoFirstScreen();
		bool isLeaveAnimation;
		bool deviceError;
		
		Timer _missedTimer;
		ButtonTex *comeBackBtn;
		Anim<float> alphaFinAnimate, alphaFlashAnim;
		Texture bg;
};