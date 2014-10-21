#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "Saver.h"
#include "Playerdata.h"

namespace gameStates
{
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
}

using namespace ci;
using namespace gameStates;

class Game
{
	public:	

		static Game& getInstance() { static Game game; return game; };

		static const int	PREWAITING_TIME = 2;
		static const int	PREGAME_TIME	= 3;
		static const int	ONE_POSE_TIME	= 20;
		static const int	RESULT_TIME		= 4;	
		static const int	MATCHING_MAX_VALUE = 100;		

		int state;
		bool isGameRunning, isPoseDetecting, winAnimationFinished;
		ci::Timer _preWaitingTimer, _preGameTimer, _onePoseTimer, _resultTimer;
		
		Pose* foundPose, currentPose;
		std::vector<Pose*> poses;

		int poseCode, level, levelCompletion;
		float mathPercent;		

		boost::signals2::signal<void(void)> gotoResultScreenEvent;	
		boost::signals2::signal<void(void)> photoFlashEvent;	
		boost::signals2::signal<void(void)> levelCompleteEvent, gameOverEvent;
		
		void setup()
		{
			poses = saver().loadPoseBase();
		}

		void initnew()
		{
			state = SHOW_FIRST_MESSAGE;
			kinect().startTracking();			
			PlayerData::initData();	

			level = 1;
			poseCode = generatePoseCode();	

			_preWaitingTimer.start();
		}

		int generatePoseCode()
		{
			return level - 1; //TODO
		}		

		void update()
		{
			switch(state)
			{
				case SHOW_FIRST_MESSAGE:
					updateFirstMessage();
				break;

				case PRE_GAME_INTRO:
					updatePreGameIntro();
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
					makeScreenShootUpdtae();
				break;
			}
		}

		void updateFirstMessage() 
		{
			if (preWaitingTimerIsFinished() && kinect().getSkeletsInFrame() >0)
			{
				_preWaitingTimer.stop();
				_preGameTimer.start();
				state = PRE_GAME_INTRO;				
			}
		}

		void updatePreGameIntro() 
		{
			if (preGameTimerIsFinished())
			{
				_preGameTimer.stop();
				_onePoseTimer.start();		
				state = MAIN_GAME;
				isPoseDetecting = false;
				isGameRunning   = true;
			}
		}

		void updateMainGame() 
		{
			if (mainTimerIsFinished())
			{
				stopPersonChecking();
				isPoseDetecting = false;
			}
			else
			{
				checkPersonPose();
			}
		}

		void checkPersonPose() 
		{
			updateGame();
	
			if (state == MAIN_GAME)/// getPoseProgress() >= MATCHING_MAX_VALUE)
			{
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

				if (mathPercent > Params::percentForMatching )		
				{
					levelCompletion += 4;
				}
				else
				{
					if(levelCompletion - 6 >=0)
						levelCompletion -= 6;
					else
						levelCompletion = 0;
				}
			}	
		}

		void updateGameResult() 
		{
			if(showGameResultTimeIsFinished())
			{
				_resultTimer.stop();
				state = MAKE_SCREENSHOOT;			
			}
		}

		void updatePhotoMaking() 
		{
			if (cameraCanon().checkIfDownloaded())
			{
				setPlayerOnePoseGuess(cameraCanon().getpathToDownloadedPhoto());			
				checkAnimationFinished();
			}
			else if (cameraCanon().checkIfError())
			{
				setPlayerOnePoseGuess();
				checkAnimationFinished();
			}
		}

		void checkAnimationFinished() 
		{
			state = winAnimationFinished ? SHOW_GAME_RESULT: WIN_ANIMATION_FINISH_WAIT;
		}

		void updateAnimationWait() 
		{
			if (winAnimationFinished)
				state = SHOW_GAME_RESULT;
		}

		void setPlayerOnePoseGuess(std::string pathToHiRes = "") 
		{
			PlayerData::score++;			
			PlayerData::playerData[level ].isFocusError = false;
			PlayerData::playerData[level ].isSuccess	 = true;
			PlayerData::playerData[level ].pathHiRes	 =  pathToHiRes;
			PlayerData::playerData[level ].screenshot	 = cameraCanon().getSurface();
			PlayerData::playerData[level ].storyCode	 = poseCode;
		}

		void makeScreenShootUpdtae() 
		{
			if (level++ >= POSE_IN_GAME_TOTAL)
			{						
				state = NONE;
				gotoResultScreenEvent();
			}
			else
			{				
				poseCode = generatePoseCode();	
				state = PRE_GAME_INTRO;
				_preGameTimer.start();
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

	/*	void saveAsTemplate(string name)
		{
			if (currentSkelet.size() && savePoseDepth) 
			{
				Pose* pose = new Pose();
				pose->setName(name);
				pose->setPoints(currentSkelet);
				pose->setKinectTilt(getTilt());
				pose->setImage(savePoseDepth);
				saver().savePoseIntoBase(pose);		
			}
		}*/

		void matchTemplate()
		{
			foundPose = NULL;

			currentPose.setPoints(kinect().getCurrentSkelet());
			currentPose.createBoundingBox();
			currentPose.createNormalizePoints();
			currentPose.initColors();

			if(!isPointsSizeEqual(*poses[poseCode], currentPose))
			{
				mathPercent = 0;
				return;
			}	

			if (!isAnchorPointsMatch(*poses[poseCode], currentPose))
			{
				mathPercent = 0;
				return;
			}	

			//computeMistakeWay1();
			computeMistakeWay2();
		}
		
		void stopPersonChecking() 
		{
			_onePoseTimer.stop();
			_resultTimer.start();
			isGameRunning = false;
			state = SHOW_GAME_RESULT;
		}

		void computeMistakeWay1()
		{
			double min_dist			 = FLT_MAX;	
			double maxErrorToDiscard = 1000;
			double sum_mistake		 = 0;

			for(size_t j = 0, len = currentPose.getNormalizePoints().size(); j < len ; ++j) 
			{
				double mistake = calculateDistanceBetweenPoints(poses[poseCode]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
	
				sum_mistake += mistake;

				if (mistake > Params::maxErrorBetweenJoints) 
				{
					currentPose.setPointColor(j, Color(1,1,1));
					//mathPercent = 0;
					//return;
					//break;
				}
			}

			double dist = sum_mistake/currentPose.getNormalizePoints().size();

			if(dist >= 0 && dist < min_dist) 
			{
				min_dist = dist;
				foundPose = poses[poseCode];
			}

			double kv = BOX_SCALE*BOX_SCALE;
			double half_diagonal = 0.5 * sqrt(kv + kv); 
			mathPercent = 1.0 - (min_dist/half_diagonal);	

			mathPercent < 0 ? mathPercent = 0 : mathPercent = mathPercent;
		}

		void computeMistakeWay2()
		{
			mathPercent = 0;
			size_t len = currentPose.getNormalizePoints().size();
			double onePart = 100.0 / len;

			for(size_t j = 0; j < len ; ++j) 
			{
				double mistake = calculateDistanceBetweenPoints(poses[poseCode]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
				double onePartPercent = 0;

				if (mistake >= Params::maxErrorBetweenJoints) 
				{
					onePartPercent = 0;
				}
				else
				{			
					onePartPercent = onePart * (1-mistake/Params::maxErrorBetweenJoints);
				}
				mathPercent += onePartPercent;
			}
			mathPercent*=0.01;
		}

		double getMatchPercent()
		{
			return mathPercent*100;
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
			return (Vec2f(vec1.x, vec1.y) - Vec2f(vec2.x, vec2.y) ).length();
		}

		void stopAllTimersIfNeed()
		{
			if(!_preWaitingTimer.isStopped()) _preWaitingTimer.stop();
			if(!_preGameTimer.isStopped()) _preGameTimer.stop();
			if(!_onePoseTimer.isStopped()) _onePoseTimer.stop();
			if(!_resultTimer.isStopped()) _resultTimer.stop();			
		}
		
		bool preWaitingTimerIsFinished() 
		{
			return  (!_preWaitingTimer.isStopped() &&  _preWaitingTimer.getSeconds() > PREWAITING_TIME);
		}

		bool preGameTimerIsFinished() 
		{	
			return  (!_preGameTimer.isStopped()	  &&  _preGameTimer.getSeconds() > PREGAME_TIME);
		}

		bool mainTimerIsFinished() 
		{
			return  (!_onePoseTimer.isStopped() &&  _onePoseTimer.getSeconds() > ONE_POSE_TIME);
		}

		bool showGameResultTimeIsFinished() 
		{
			return  (!_resultTimer.isStopped() &&  _resultTimer.getSeconds() > RESULT_TIME);
		}

		void flashAnimationFinished()
		{
			winAnimationFinished = true;
			_resultTimer.start();	
		}	
};

inline Game&	recognitionGame() { return Game::getInstance(); };