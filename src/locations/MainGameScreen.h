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
#include "ScreenshotHolder.h"
#include "PlayerData.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace MainGameDefaults
{	
	enum states {
				 NONE, 
				 SHOW_FIRST_MESSAGE,
				 PRE_GAME_INTRO,
				 MAIN_GAME, 
				 SHOW_GAME_RESULT
	};
}

class MainGameScreen : public Location
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

		static MainGameScreen* Instance() {
			return &MainGameScreenState;
		}

	protected:
		MainGameScreen() { }

	private:

		LocationEngine*			_game;
		static MainGameScreen	MainGameScreenState;

		int						state;
		std::map<int, string>	stateMemoMap;

		void					checkPersonMissed();

		bool					preWaitingTimerIsFinished();
		bool					personisFound();
		bool					preGameTimerIsFinished();		
		void					checkPersonPose();
		bool					mainTimerIsFinished();
		bool					showGameResultTimeIsFinished();
		bool					allPoseDone();
		void					gotoResultScreen();
		void					gotoIntroState();
		void					nextPose();

		void					updateDebugMessage();
		void					drawDebugMessage();

		ci::Font				debugFont26, debugFont46;

		int						peopleCount;
		float					distanceToPlayer;

		ci::Timer				_preWaitingTimer, _preGameTimer, _onePoseTimer, _resultTimer, _missedTimer;

		static const int		PREWAITING_TIME = 2;
		static const int		PREGAME_TIME	= 3;
		static const int		ONE_POSE_TIME	= 20;
		static const int		RESULT_TIME		= 4;
		static const int		MAX_MISSED_TIME	= 20;		

		int						currentPose;
		static const int		ALL_POSES_COUNT	 = 3;

		void					drawCameraImage();
		void					drawFirstMessageBox();
		void					drawPreReadyCounterBox();
		void					drawPoseSilhouette();
		void					drawGameResult();
		void					drawPoseComics();
		void					stopPersonChecking();
		
	
		static KinectAdapter*	kinect;

		static const int		MATCHING_MAX_VALUE = 100;

		ci::gl::Texture failImage, comics1_godzilla, comics2_cat, comics3_uni;
	
};