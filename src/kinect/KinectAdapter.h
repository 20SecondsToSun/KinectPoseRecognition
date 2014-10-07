#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "Utils.h"
#include "Kinect.h"
#include "Params.h"
#include "Pose.h"
#include "KinectBase.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

class KinectAdapter: public KinectBase
{
	public:
		boost::signals2::signal<void(void)>		levelCompleteEvent, gameOverEvent;
		void Setup();	
		void update();
		void draw();

		static KinectAdapter* Instance() {
			return &KinectAdapterState;
		}	

		void						saveAsTemplate(std::string name);
		void						matchTemplate();	

		std::vector<Pose*>			poses;		
	
		void						startPoseGame();
		void						nextPose();
		std::string					getPoseIndex();
		void						drawPoseComics();
		void						poseComplete();
		float						getPoseProgress();	
		void						drawLoadedPoses();

		int							getSkeletsInFrame();		
		void						drawSkeletJoints();

		bool						isGameRunning, isPoseDetecting;
		double						getMatchPercent();

protected:
		KinectAdapter() { }

	private:
		static KinectAdapter		KinectAdapterState;		

		void						updateSkeletonData();	

		std::vector<ci::Vec3f>		currentSkelet;		
		
		Pose*						foundPose;
		float						mathPercent;
		void						drawUserMask();

		std::vector<_NUI_SKELETON_POSITION_INDEX>			jointToRecord;
		Surface16u					savePoseDepth;		
		int							currentPoseInGame;
		int							levelCompletion;
		void						setActiveJoints();
		
		bool						isAnchorPointsMatch(Pose pose1, Pose pose2);
		bool						isPointsSizeEqual(Pose pose1, Pose pose2);
		double						calculateDistanceBetweenPoints(ci::Vec3f vec1, ci::Vec3f vec2);

		Pose						currentPose;

		void						computeMistakeWay1();
		void						computeMistakeWay2();
};

inline KinectAdapter&	kinect() { return *KinectAdapter::Instance(); };