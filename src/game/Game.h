#pragma once
#pragma warning(push)
#pragma warning(disable: 4244)

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "Saver.h"
#include "Playerdata.h"
#include "KinectAdapter.h"
#include "HintScreen.h"
#include "GameControLScreen.h"
#include "ComicsScreen.h"

namespace gameStates
{
	enum states {
		NONE, 
		STEP_BACK_MESSAGE,
		HINT_MESSAGE,
		PRE_GAME_INTRO,
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
using namespace std;
using namespace gameStates;

class Game
{
public:	

	static Game& getInstance() { static Game game; return game; };

	static const int	STEP_BACK_TIME  = 2;	
	static const int	PREGAME_TIME	= 2;
	static const int	COUNTDOWN_TIME	= 3;
	static const int	HINT_TIME		= 6;
	static const int	RESULT_TIME		= 4;
	static const int	COUNTERS_ANIM_TIME	= 2;

	static const int	LEVEL_PASS_INC	= 4;
	static const int	LEVEL_PASS_DEC	= 6;

	int				CURRENT_POSE_TIME;
	int				CURRENT_MATCHING_PERCENT;

	int state;
	bool isGameRunning, isPoseDetecting, winAnimationFinished;
	Timer  _preGameTimer, _onePoseTimer, _resultTimer, _stepBackTimer, _hintTimer, _countDownTimer, _countersAnimTimer;

	Pose* foundPose, currentPose;
	vector<Pose*> poses;

	int poseCode, level, levelCompletion;
	float mathPercent;		

	boost::signals2::signal<void(void)> gotoResultScreenEvent;	
	boost::signals2::signal<void(void)> photoFlashEvent;

	ci::signals::connection quickAnimationFinishedSignal;

	int poseNum;

	void setup()
	{
		poses = saver().loadPoseBase();
		poseNum = 0;
	}

	void initnew()
	{
		state = STEP_BACK_MESSAGE;
		kinect().startTracking();			
		PlayerData::initData();	

		level = 1;
		poseCode = generatePoseCode();	
		gameControls().setCurrentPose(poses[poseCode]);
		CURRENT_POSE_TIME = poses[poseCode]->getPoseTime();
		CURRENT_MATCHING_PERCENT = poses[poseCode]->getPercent();

		_stepBackTimer.start();
	}

	void update()
	{
		switch(state)
		{
		case STEP_BACK_MESSAGE:
			updateStepBackMessage();
			break;

		case HINT_MESSAGE:
			updateHintMessage();
			break;			

		case PRE_GAME_INTRO:
			updatePreGameIntro();
			break;

		case COUNTER_STATE:
			updateCounterState();
			break;

		case COUNTERS_ANIMATE:
			updateCountersAnimState();
			break;

		case MAIN_GAME:			
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

	int generatePoseCode()
	{
		int code = poseNum++;			
		if (poseNum > poses.size() - 1)
			poseNum = 0;

		return code;//level - 1; //TODO
	}			

	void updateStepBackMessage() 
	{
		updateGame();

#ifdef debug
		if(isTimerFinished(_stepBackTimer, STEP_BACK_TIME)) 
#else
		if(isTimerFinished(_stepBackTimer, STEP_BACK_TIME) && kinect().distanceToSkelet() > MIN_DISTANCE_TO_SKELET)
#endif
		{
			_preGameTimer.start();

			hintScreen().poseNum = level;
			hintScreen().startReadySate();
			state = PRE_GAME_INTRO;
		}
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
				state = HINT_MESSAGE;
			}
			else
			{	
				gameControls().show();
				_countersAnimTimer.start();
				hintScreen().fadeCounter();
				state = COUNTERS_ANIMATE;
			}
		}
		else
		{
			hintScreen().updateCountDown( COUNTDOWN_TIME-(int)_countDownTimer.getSeconds());
		}
	}

	void updateCountersAnimState() 
	{		
		if (isTimerFinished(_countersAnimTimer, COUNTERS_ANIM_TIME))
		{	
			_onePoseTimer.start();	

			isPoseDetecting = false;
			isGameRunning   = true;
			gameControls().quickAnimationSetProgress(1);
			state = MAIN_GAME;	
		}
		else
		{
			gameControls().quickAnimationSetProgress(_countersAnimTimer.getSeconds());
		}
	}

	void updateMainGame() 
	{
		if (isTimerFinished(_onePoseTimer, CURRENT_POSE_TIME))
		{
			gameControls().setShowingTime(0);
			stopPersonChecking();
			isPoseDetecting = false;
			gotoLevelCompleteScreen() ;
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
			poseCode = generatePoseCode();	

			_preGameTimer.start();	
			hintScreen().poseNum = level;
			hintScreen().startReadySate();
			gameControls().setCurrentPose(poses[poseCode]);
			CURRENT_POSE_TIME = poses[poseCode]->getPoseTime();
			CURRENT_MATCHING_PERCENT= poses[poseCode]->getPercent();
			state = PRE_GAME_INTRO;
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
		}			
		comicsScreen().show();

		state = SHOW_GAME_RESULT;	
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
		kinect().updateSkeletonData();

		if (isGameRunning)
		{
			matchTemplate();

#ifdef alwayswin
			mathPercent = 1;
#endif
			//mathPercent = 1;

			if (mathPercent > Params::percentForMatching )
			{
				levelCompletion += LEVEL_PASS_INC;
			}
			else
			{
				if(levelCompletion - LEVEL_PASS_DEC >= 0 )
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

	ci::gl::Texture getPoseImage()
	{
		return poses[poseCode]->getComicsImage();
	}

	void drawCurrentSilhouette()
	{
		return poses[poseCode]->draw();
	}

	ci::gl::Texture getPoseImageById(int id)
	{
		return poses[id]->getComicsImage();
	}

	string getPoseIndex()
	{
		return to_string(poses.size() + 1);
	}

	ci::Surface getCurrentScreenShot() 
	{
		return PlayerData::playerData[level].screenshot;
	}	

	void matchTemplate()
	{
		foundPose = NULL;

		currentPose.setPoints(kinect().getCurrentSkelet());
		currentPose.createNormalizePoints0();
		currentPose.initColors();

		if(!isPointsSizeEqual(*poses[poseCode], currentPose))
		{
			mathPercent = 0.0f;
			return;
		}

		computeMistakeWay1();
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
				currentPose.setPointColor(j, ColorA(1.0f, 1.0f, 1.0f, 0.0f));					
				onePartPercent = onePart * (1 - mistake / (Params::maxErrorBetweenJoints - Params::minErrorBetweenJoints));
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

		return (abs(anchor1.x - anchor2.x) < Params::boxMaxErrorX && abs(anchor1.y -anchor2.y) < Params::boxMaxErrorY  );
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
		gl::pushMatrices();
		gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
		gl::scale( kinect().headScale,  kinect().headScale);
		currentPose.drawPoints();
		poses[poseCode]->drawPoints();
		gl::popMatrices();
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