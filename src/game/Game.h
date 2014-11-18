#pragma once
#pragma warning(push)
#pragma warning(disable: 4244)

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "Saver.h"
#include "Playerdata.h"
#include "KinectAdapter.h"
#include "HintScreen.h"
#include "GameControlScreen.h"
#include "ComicsScreen.h"

namespace gameStates
{
	enum states
	{
		NONE, 
		STEP_BACK_MESSAGE,
		HINT_MESSAGE,
		PRE_GAME_INTRO,
		HANDS_UP_AWAITING,
		MAIN_GAME, 
		COUNTERS_ANIMATE,
		SHOW_GAME_RESULT,
		PHOTO_MAKING_WAIT,
		WIN_ANIMATION_FINISH_WAIT,
		MAKE_SCREENSHOOT,
		COUNTER_STATE
	};
}

using namespace ci;
using namespace ci::gl;
using namespace std;
using namespace gameStates;

class Game
{
public:	

	static Game& getInstance() { static Game game; return game; };

	static const int	STEP_BACK_TIME  = 10;
	static const int    HANDS_UP_AWAITINTING_TIME = 15;
	static const int    STEP_BACK_ENSURE_TIME = 2;

	static const int	PREGAME_TIME		= 2;
	
	static const int	COUNTDOWN_TIME		= 3;
	static const int	HINT_TIME			= 3;
	static const int	RESULT_TIME			= 4;
	static const int	COUNTERS_ANIM_TIME	= 2;

	static const int	LEVEL_PASS_INC		= 4;
	static const int	LEVEL_PASS_DEC		= 6;

	int CURRENT_POSE_TIME;
	int CURRENT_MATCHING_PERCENT;
	int state;
	int allHumanPointsInScreenRectStable;

	bool isGameRunning, isPoseDetecting, winAnimationFinished, _onePoseTimerPause;
	Timer  _preGameTimer, _onePoseTimer, _resultTimer, _stepBackTimer, _hintTimer;
	Timer _handsUpAwaitingTimer, _stepBackEnsureTimer, _countDownTimer, _countersAnimTimer;

	Pose* foundPose, currentPose;
	vector<Pose*> poses;

	int poseNum, poseCode, level, levelCompletion;
	float mathPercent, etalonHeight;	
	float scaleAccordingUserHeight;

	boost::signals2::signal<void(void)> gotoResultScreenEvent;
	boost::signals2::signal<void(void)> gotoFirstScreenEvent;	
	boost::signals2::signal<void(void)> photoFlashEvent;
	boost::signals2::signal<void(void)> cameraStartUpdateEvent;
	boost::signals2::signal<void(void)> cameraStopUpdateEvent;

	ci::signals::connection quickAnimationFinishedSignal;
	
	bool testPercent100, isHandsUp;	
	string debugString;

	std::vector<ci::Vec3f> skelet;

	Rectf districtKinectRectf;

	void setup()
	{
		poses = saver().loadPoseBase();
		poseNum = 0;
		testPercent100 = false;

		kinect().gestureEvent.connect( [ & ]( )
		{
			console()<<"up--------------------------->  "<<endl;
			isHandsUp = true;
		});	
	}

	void initnew()
	{		
		kinect().startTracking();
		PlayerData::initData();

		level = 1;
		scaleAccordingUserHeight = 1.0f;
		initPoseData();

		_stepBackTimer.start();
		_stepBackEnsureTimer.start();
		state = STEP_BACK_MESSAGE;
		districtKinectRectf = Rectf( 500, 0 , 1200, 1080);
	}	

	void update()
	{
		kinect().updateSkeletonData(districtKinectRectf);		
		skelet = kinect().getCurrentSkelet();

		switch(state)
		{
		case STEP_BACK_MESSAGE:
			districtKinectRectf = Rectf( 500, 0 , 1200, 1080);
			updateStepBackMessage(); 
			break;

		case HINT_MESSAGE:
			updateHintMessage();
			break;

		case PRE_GAME_INTRO:
			updatePreGameIntro();
			break;

		case HANDS_UP_AWAITING:
			districtKinectRectf = Rectf( 500, 0 , 1200, 1080);
			updateHandsAwaiting();
			break;			

		case COUNTER_STATE:
			updateCounterState();
			break;

		case COUNTERS_ANIMATE:
			updateCountersAnimState();
			break;

		case MAIN_GAME:
			districtKinectRectf = Rectf( 200, 0 , 1700, 1080);
			updateMainGame();
			break;

		case SHOW_GAME_RESULT:
			updateGameResult();
			break;

		case PHOTO_MAKING_WAIT:
			updatePhotoMaking();
			break;

		case WIN_ANIMATION_FINISH_WAIT:
			updateAnimationWait();
			break;

		case MAKE_SCREENSHOOT:
			makeScreenShootUpdate();
			break;
		}		
	}
	
	void updateStepBackMessage() 
	{
		if (allStpeBackConditionsGood())
		{			
			hintScreen().startHandsAwaiting();
			_handsUpAwaitingTimer.start();
			kinect().enbleGestures();
			isHandsUp = false;
			allHumanPointsInScreenRectStable = 0;
			state = HANDS_UP_AWAITING;
		}
		else if(isTimerFinished(_stepBackTimer, STEP_BACK_TIME))
		{
			gotoFirstScreenEvent();
		}
	}

	bool allStpeBackConditionsGood()
	{
		//return true;
		return isTimerFinished(_stepBackEnsureTimer, STEP_BACK_ENSURE_TIME)
			&& kinect().isDistanceOk()
			&& kinect().allHumanPointsInScreenRect();
			//&& kinect().isHeapInRect(districtKinectRectf);
	}

	void updateHandsAwaiting() 
	{
		if(!kinect().allHumanPointsInScreenRect())
			allHumanPointsInScreenRectStable++;

		if (!kinect().isDistanceOk() || allHumanPointsInScreenRectStable > 10)
		{
			_stepBackTimer.start();
			_stepBackEnsureTimer.start();
			_handsUpAwaitingTimer.stop();
			kinect().disableGestures();
			hintScreen().init();
			state = STEP_BACK_MESSAGE;
		}
		else if (isTimerFinished(_handsUpAwaitingTimer, HANDS_UP_AWAITINTING_TIME))
		{
			gotoFirstScreenEvent();
			kinect().disableGestures();		
		}
		else if (isHandsUp)
		{
			scaleAccordingUserHeight = 1.0f;

			float etalonHeight    =  kinect().getEtalonHeightInPixelsAccordingDepth();
			float userHeight      =  kinect().userHeightInPixels();
	
			if(etalonHeight)
				scaleAccordingUserHeight = Utils::clamp(userHeight / etalonHeight, 1.0f, 0.6f);
			else
				scaleAccordingUserHeight = 1.0f;

			debugString = "SCALE: " + to_string(scaleAccordingUserHeight) +"\nuserHeight: "+ to_string((int)userHeight)+"\netalonHeight: "+ to_string((int)etalonHeight);
			//scaleAccordingUserHeight = 0.8f;
			hintScreen().poseNum = level;
			hintScreen().startReadySate();

			poses[poseCode]->calculateShifts(scaleAccordingUserHeight);
			poses[poseCode]->scalePoints(scaleAccordingUserHeight);

			gameControls().setPoseScale(scaleAccordingUserHeight);
			gameControls().setPoseShift(poses[poseCode]->getPoseShift());

			_preGameTimer.start();
			kinect().disableGestures();
			state = PRE_GAME_INTRO;
		}		
		//else
		//{
		//	//allHumanPointsInGame();
		//	//float  userHeight = 0;//floor(currentPose.calculateUserHeight());
		//	//debugString = " curHeight:  "+ to_string(userHeight) + " |||| distance: "+to_string( kinect().distanceToSkelet());
		//}
	}
	
	void updatePreGameIntro() 
	{
		if (isTimerFinished(_preGameTimer, PREGAME_TIME))
		{
			_countDownTimer.start(); 

			hintScreen().startCountDown();
			state = COUNTER_STATE;
		}
	}	

	void updateCounterState() 
	{	
		if (isTimerFinished(_countDownTimer, COUNTDOWN_TIME))
		{
			gameControls().setTime(CURRENT_POSE_TIME);
			gameControls().setQuickAnimTime(CURRENT_POSE_TIME + 40);
			gameControls().setQuickAnimPosePercent(.41f);	
			gameControls().showSilhouette();

			if (level == 1)
			{
				_hintTimer.start();
				hintScreen().startHint();
				gameControls().show1();

				cameraStartUpdateEvent();
				state = HINT_MESSAGE;
			}
			else
			{	
				gameControls().show();
				_countersAnimTimer.start();
				hintScreen().fadeCounter();

				cameraStartUpdateEvent();
				state = COUNTERS_ANIMATE;
			}
		}
		else
		{
			hintScreen().updateCountDown( COUNTDOWN_TIME - (int)_countDownTimer.getSeconds());
		}
	}

	void updateCountersAnimState() 
	{		
		if (isTimerFinished(_countersAnimTimer, COUNTERS_ANIM_TIME))
		{	
			_onePoseTimer.start();

			isPoseDetecting = false;
			isGameRunning   = true;
			_onePoseTimerPause = false;
			mathPercent = 0.0f;
			state = MAIN_GAME;	
		}
		else
		{
			gameControls().quickAnimationSetProgress(_countersAnimTimer.getSeconds());
		}
	}

	void updateMainGame() 
	{
		if (isTimerFinished(_onePoseTimer, CURRENT_POSE_TIME) && !_onePoseTimerPause)
		{
			gameControls().setShowingTime(0);
			stopPersonChecking();
			isPoseDetecting = false;
			gotoLevelCompleteScreen();
		}
		else if (_onePoseTimerPause)
		{
			gameControls().setShowingTime(CURRENT_POSE_TIME);
			gameControls().setDetentionPercent(mathPercent);
			checkPersonPose();
		}
		else
		{
			gameControls().setShowingTime(CURRENT_POSE_TIME - (int)_onePoseTimer.getSeconds());
			gameControls().setDetentionPercent(mathPercent);
			checkPersonPose();
		}
	}

	void updateHintMessage() 
	{
		if(isTimerFinished(_hintTimer, HINT_TIME))
		{				
			_countersAnimTimer.start();
			hintScreen().fadeHint();
			gameControls().show2();
			state = COUNTERS_ANIMATE;
		}
	}	

	void makeScreenShootUpdate() 
	{
		if (level++ >= POSE_IN_GAME_TOTAL)
		{
			state = NONE;
			gotoResultScreenEvent();
		}
		else
		{
			hintScreen().poseNum = level;
			hintScreen().startReadySate();
			initPoseData();

			state = PRE_GAME_INTRO;
			_preGameTimer.start();
		}
	}

	void updateGameResult() 
	{
		if(isTimerFinished(_resultTimer, RESULT_TIME))
		{
			_resultTimer.stop();
			state = MAKE_SCREENSHOOT;
		}
	}

	void updatePhotoMaking() 
	{
		if (cameraCanon().checkIfDownloaded())
		{
			console()<<"path ::: "<<cameraCanon().getpathToDownloadedPhoto()<<endl;
			setPlayerOnePoseGuess(cameraCanon().getpathToDownloadedPhoto());
			checkAnimationFinished();
		}
		else if (cameraCanon().checkIfError())
		{
			setPlayerOnePoseGuess("", true);
			checkAnimationFinished();
		}
	}

	void checkAnimationFinished() 
	{
		if (winAnimationFinished)
			gotoLevelCompleteScreen(); 
		else 
			state = WIN_ANIMATION_FINISH_WAIT;
	}

	void updateAnimationWait() 
	{
		if (winAnimationFinished)
			gotoLevelCompleteScreen(); 
	}

	void gotoLevelCompleteScreen() 
	{
		gameControls().hide();

		comicsScreen().isGuess = isPoseDetecting;
		if(isPoseDetecting)	
		{
			comicsScreen().comicsTexture = PlayerData::playerData[level-1].screenshot;
			comicsScreen().poseTexture   = getPoseImage();
			comicsScreen().setPoseScale(scaleAccordingUserHeight);
			comicsScreen().setPoseShift(poses[poseCode]->getPoseShift());
			comicsScreen().show();
			//state = SHOW_GAME_RESULT;	
			state = NONE;
			gotoResultScreenEvent();
		}
		else
		{
			state = NONE;
			gotoResultScreenEvent();
		}
	}

	void flashAnimationFinished()
	{
		winAnimationFinished = true;
		_resultTimer.start();	
	}	

	///////////////////////////////////////////////////////////////////
	//
	//			POSE DETECTION
	//
	///////////////////////////////////////////////////////////////////

	void initPoseData()
	{		
		poseCode = generatePoseCode();

		CURRENT_POSE_TIME = poses[poseCode]->getPoseTime();
		CURRENT_MATCHING_PERCENT = poses[poseCode]->getPercent();
		poses[poseCode]->calculateShifts(scaleAccordingUserHeight);
		poses[poseCode]->scalePoints(scaleAccordingUserHeight);

		gameControls().setCurrentPose(poses[poseCode]);
		gameControls().setPoseScale(scaleAccordingUserHeight);
		gameControls().setPoseShift(poses[poseCode]->getPoseShift());
	}

	int generatePoseCode()
	{
		int code = poseNum++;
		if (poseNum > poses.size() - 1)
			poseNum = 0;

		return code;//level - 1; //TODO
	}

	void checkPersonPose() 
	{
		updateGame();

		if ( getPoseProgress() >= CURRENT_MATCHING_PERCENT + 10)
		{
			gameControls().showMatching(1.0f);
			isPoseDetecting		 = true;
			isGameRunning		 = false;
			winAnimationFinished = false;	

			levelCompletion = 0;

			stopPersonChecking();
			_onePoseTimer.stop();

			state = PHOTO_MAKING_WAIT;
			photoFlashEvent();
			cameraCanon().takePhoto();
		}
	}

	void updateGame() 
	{
		//kinect().updateSkeletonData();

		if (isGameRunning)
		{
			matchTemplate();

			if (testPercent100) 
				mathPercent = 1;

			if (mathPercent > Params::percentForMatching)
			{
				levelCompletion += LEVEL_PASS_INC;

				if (_onePoseTimerPause == false)
				{
					CURRENT_POSE_TIME -= (int)_onePoseTimer.getSeconds();

					if (_onePoseTimer.isStopped() == false)
						_onePoseTimer.stop();

					_onePoseTimerPause = true;
				}
			}
			else
			{
				if (_onePoseTimer.isStopped() == true)
					_onePoseTimer.start();

				_onePoseTimerPause = false;

				if(levelCompletion - LEVEL_PASS_DEC >= 0)
					levelCompletion -= LEVEL_PASS_DEC;
				else
					levelCompletion = 0;
			}

			gameControls().showMatching((float)levelCompletion / CURRENT_MATCHING_PERCENT);
		}	
	}

	float getPoseProgress() 
	{
		return floor(levelCompletion);
	}

	Texture getPoseImage()
	{
		return poses[poseCode]->getComicsImage();
	}

	void drawCurrentSilhouette()
	{
		return poses[poseCode]->draw();
	}

	Texture getPoseImageById(int id)
	{
		return poses[id]->getComicsImage();
	}

	string getPoseIndex()
	{
		return to_string(poses.size() + 1);
	}

	Surface getCurrentScreenShot() 
	{
		return PlayerData::playerData[level].screenshot;
	}	

	void matchTemplate()
	{
		foundPose = NULL;

		SkeletJoints skelet = kinect().getCurrentSkelet();

		if (skelet.size() == 0)
		{
			mathPercent = 0.0f;
			return;
		}

		currentPose.setPoints(skelet);
		currentPose.createNormalizePoints0();
		currentPose.initColors();

		if(isPointsSizeEqual(*poses[poseCode], currentPose))
		{
			computeMistakeWay1();
		}
		else
		{
			mathPercent = 0.0f;
		}
	}

	void stopPersonChecking() 
	{
		_onePoseTimer.stop();
		_resultTimer.start();
		isGameRunning = false;
	}

	void computeMistakeWay1()
	{
		mathPercent = 0.0f;

		for(size_t j = 0, len = currentPose.getNormalizePoints().size(); j < len ; ++j) 
		{
			double mistake = calculateDistanceBetweenPoints(poses[poseCode]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
			double onePartPercent = 0;
			double onePart = Params::weightJoints[j];

			if (mistake >= Params::maxErrorBetweenJoints)
			{
				onePartPercent = 0;
			}
			else if (mistake < Params::minErrorBetweenJoints)
			{
				currentPose.setPointColor(j, ColorA(1.0f, 1.0f, 1.0f, 0.0f));
				onePartPercent = onePart;
			}
			else
			{
				float norma = (Params::maxErrorBetweenJoints - mistake) / (Params::maxErrorBetweenJoints - Params::minErrorBetweenJoints);
				onePartPercent = onePart * (1 - norma);
				currentPose.setPointColor(j, ColorA(0.0f, 0.0f, 1.0f, abs(onePartPercent*10)));
			}

			mathPercent += onePartPercent;
		}
	}

	double getMatchPercent()
	{
		return mathPercent * 100;
	}

	bool isPointsSizeEqual(Pose pose1, Pose pose2)
	{
		return pose1.getNormalizePoints().size() == pose2.getNormalizePoints().size();
	}

	bool isAnchorPointsMatch(Pose pose1, Pose pose2)
	{
		Vec2f anchor1 = pose1.getAnchor();
		Vec2f anchor2 = pose2.getAnchor();

		return (abs(anchor1.x - anchor2.x) < Params::boxMaxErrorX && abs(anchor1.y - anchor2.y) < Params::boxMaxErrorY);
	}

	double calculateDistanceBetweenPoints(Vec3f vec1, Vec3f vec2)
	{
		return abs((Vec2f(vec1.x, vec1.y) - Vec2f(vec2.x, vec2.y) ).length());
	}

	void setPlayerOnePoseGuess(std::string pathToHiRes = "" , bool _focusError = false) 
	{	
		PlayerData::score++;
		PlayerData::playerData[level-1].isFocusError  = _focusError;
		PlayerData::playerData[level-1].isSuccess	  = true;
		PlayerData::playerData[level-1].pathHiRes	  =  pathToHiRes;
		PlayerData::playerData[level-1].screenshot	  = cameraCanon().getSurface();
		PlayerData::playerData[level-1].storyCode	  = poseCode;

		if (_focusError == false) PlayerData::photosWithoutError++;
	}

	void drawJoints()
	{
		// compare pose points
		gl::pushMatrices();
		gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
		gl::scale( kinect().headScale,  kinect().headScale);
		//poses[poseCode]->drawRawPoints();
		//gl::color(Color::hex(0xF5F5DC));
		poses[poseCode]->drawPoints();
		gl::popMatrices();
	}

	void drawCurrentPlayerJoints()
	{
		gl::drawStrokedRect(districtKinectRectf);
		if (skelet.size() == 0)
			return;
		
		// current player points
		gl::pushMatrices();
		gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
		gl::scale( kinect().headScale,  kinect().headScale);
		currentPose.setPoints(skelet);
		currentPose.createNormalizePoints0();
		currentPose.initColors();
		currentPose.drawPoints();
		gl::popMatrices();
		Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.0f, 40.0f), Color(1.0f, 0.0f, 0.0f));
	}

	///////////////////////////////////////////////////////////////////
	//
	//			TIMERS
	//
	///////////////////////////////////////////////////////////////////

	void stopAllTimersIfNeed()
	{
		if(!_stepBackTimer.isStopped()) _stepBackTimer.stop();
		if(!_hintTimer.isStopped()) _hintTimer.stop();
		if(!_preGameTimer.isStopped()) _preGameTimer.stop();
		if(!_countDownTimer.isStopped()) _countDownTimer.stop();
		if(!_countersAnimTimer.isStopped()) _countersAnimTimer.stop();
		if(!_onePoseTimer.isStopped()) _onePoseTimer.stop();
		if(!_resultTimer.isStopped()) _resultTimer.stop();
		if(!_handsUpAwaitingTimer.isStopped()) _handsUpAwaitingTimer.stop();
		if(!_stepBackEnsureTimer.isStopped()) _stepBackEnsureTimer.stop();
	}

	bool isTimerFinished(Timer timer, int endTime) 
	{
		if(timer.isStopped() || (!timer.isStopped() &&  timer.getSeconds() > endTime))
		{	
			timer.stop();
			return true;
		}

		return false;
	}

	///////////////////////////////////////////////////////////////////
	//
	//			POSE SAVING
	//
	///////////////////////////////////////////////////////////////////

	void saveAsTemplate(int num)
	{
		string poseName = "Cat" + to_string(num); // + 1;	
		if (kinect().getCurrentSkelet().size() && kinect().getSilhouette()) 
		{
			Pose* pose = new Pose();
			pose->setName(poseName);
			pose->setPoints(kinect().getCurrentSkelet());
			pose->setKinectTilt(kinect().getTilt());
			pose->setImage(kinect().getSilhouette());
			pose->setTime(30);
			pose->setPercent(80);
			pose->setComicsName("default");
			saver().savePoseIntoBase(pose);		
		}
	}
};

inline Game&	recognitionGame() { return Game::getInstance(); };

#pragma warning(pop)

//void computeMistakeWay2()
//	{
//		double min_dist			 = FLT_MAX;	
//		double maxErrorToDiscard = 1000;
//		double sum_mistake		 = 0;
//		int countMistake = 0;

//		for(size_t j = 0, len = currentPose.getNormalizePoints().size(); j < len ; ++j) 
//		{
//			double mistake = calculateDistanceBetweenPoints(poses[poseCode]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
//
//			sum_mistake += mistake;
//			//console()<< " mistake     "<<mistake<<endl;
//			if (mistake > Params::maxErrorBetweenJoints) 
//			{
//				
//				//countMistake++;
//				//mathPercent = 0;
//				//return;
//				//break;
//			}
//			else
//			{
//				currentPose.setPointColor(j, ColorA(1.0f, 1.0f, 1.0f, 0.0f));
//			}
//		}

//		if (countMistake> 3) 
//		{
//			mathPercent = 0.0f;
//			return;
//		}

//		double dist = sum_mistake / currentPose.getNormalizePoints().size();

//		if(dist >= 0 && dist < min_dist) 
//		{
//			min_dist = dist;
//			foundPose = poses[poseCode];
//		}

//		double kv = BOX_SCALE*BOX_SCALE;
//		double half_diagonal = 0.5f * sqrt(kv + kv); 
//		mathPercent = 1.0f - (min_dist / half_diagonal);	

//		mathPercent < 0.0f ? mathPercent = 0 : mathPercent = mathPercent;
//	}