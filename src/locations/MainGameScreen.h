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

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

class MainGameScreen : public Location
{
	public:
		void setup();
		void init( LocationEngine* game);
		void cleanup();

		void pause(){};
		void resume(){};

		void handleEvents(){};
		void mouseEvents(){};
		void keyEvents(){};
		void update();
		void draw();

		static MainGameScreen* Instance()
		{
			return &MainGameScreenState;
		}

	private:

		static const int MAX_MISSED_TIME	= 20;	

		LocationEngine*			_game;
		static MainGameScreen	MainGameScreenState;

		std::map<int, string>	stateMemoMap;
	
		void kinectMissPersonHandler();

		void checkPersonMissed();
		bool personisFound();		

		bool showGameResultTimeIsFinished();
		void gotoResultScreen();
		
		void drawDebugMessage();		
		void drawFirstMessageBox();
		void drawPreReadyCounterBox();
		void drawPoseSilhouette();
		void drawGameResult();
		void drawPoseComics();
		void drawGameResultWithoutTimer();
		void drawCameraLostImage();
		void drawPhotoFlash();
		void drawFadeOutIfAllow();

		void animationFinished();
		void checkAnimationFinished();
		void animationLeaveLocationFinished() ;
		bool somethingWrongWithDevices();

		void removeTimers();
		void removeHandlers();

		ci::signals::connection comeBackBtnSignal;
		ci::signals::connection cameraConnectionSignal;
		ci::signals::connection kinectConnectionSignal;
		ci::signals::connection kinectMissPersonSignal;
		ci::signals::connection kinectFindPersonSignal;
		ci::signals::connection gotoResultScreenSignal;	

		void gotoFirstScreen();
		bool isLeaveAnimation;
		bool deviceError;

		ci::Font debugFont26, debugFont46;
		ci::Timer _missedTimer;
		ci::gl::Texture failImage;
		ButtonColor *comeBackBtn;
		ci::Anim<float> alphaFinAnimate;
};