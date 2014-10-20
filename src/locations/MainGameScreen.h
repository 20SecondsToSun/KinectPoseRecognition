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
#include "PlayerData.h"
#include "CameraAdapter.h"
#include "cinder/Timeline.h"

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
		MainGameScreen() { };

	private:
		LocationEngine*			_game;
		static MainGameScreen	MainGameScreenState;

		int	 state;
		enum states {
				 NONE, 
				 SHOW_FIRST_MESSAGE,
				 PRE_GAME_INTRO,
				 MAIN_GAME, 
				 SHOW_GAME_RESULT,
				 PHOTO_MAKING_WAIT,
				 WIN_ANIMATION_FINISH_WAIT,
				 MAKE_SCREENSHOOT
		};

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
		void					drawGameResultWithoutTimer();

		void					drawCameraLostImage();
		void					drawPhotoFlash();
		void					stopAllTimersIfNeed();

		void					cameraIsConnectedNow( );

		bool					winAnimationFinished, needToSaveFocusOutPhoto;

		Anim<float>				 alphaFlashAnim;
	
		static const int		MATCHING_MAX_VALUE = 100;

		ci::gl::Texture			failImage;

		void					animationFinished();
		
		void					updateFirstMessage();		
		void					updatePreGameIntro();
		void					updateMainGame();
		void					updateGameResult();
		void					updatePhotoMaking();
		void					updateAnimationWait();
		void					makeScreenShootUpdtae();

		void					setPlayerOnePoseGuess(std::string pathToHiRes = "");
		void					checkAnimationFinished();

		ButtonColor				*comeBackBtn;

		void					gotoFirstScreen() ;
		int						poseCode;

		ci::signals::connection comeBackBtnSignal;
		ci::signals::connection cameraConnectionSignal;
		ci::signals::connection kinectConnectionSignal;

		void kinectIsConnectedNow();

		bool  somethingWrongWithDevices();
		void  animationLeaveLocationFinished() ;
		bool isLeaveAnimation;

		void drawFadeOutIfAllow();
		ci::Anim<float> alphaFinAnimate;	
};