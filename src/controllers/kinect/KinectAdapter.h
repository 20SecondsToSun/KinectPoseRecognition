#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "KinectBase.h"
#include "Utils.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

typedef std::vector<ci::Vec3f> SkeletJoints;

namespace HumanGrowth
{
	const float   MAX_USER_DISTANCE = 3.5f;
	const float	  MIN_USER_DISTANCE = 2.6f;
	const float   MAX_USER_HEIGHT   = 325.0f;
	const float   MIN_USER_HEIGHT   = 240.0f;
}

class KinectAdapter: public KinectBase
{
public:

	void setup();
	void draw();
	void update();

	static KinectAdapter* Instance()
	{
		return &KinectAdapterState;
	}

	SkeletJoints getCurrentSkelet();

	int  getSkeletsInFrame();

	void drawSkeletJoints();
	void updateSkeletonData();
	
	float distanceToSkelet();
	float calculateHumanGrowsScaleTo180sm();
	float getEtalonHeightInPixelsAccordingDepth();
	float userHeightInPixels();
	float userHeightRaw();

	bool allHumanPointsInScreenRect();
	bool isHeapInRect( Rectf rect);
	bool isHandsUp();
	bool isDistanceOk();

	Surface16u getSilhouette();

	void sleep(int seconds);
	void sleepKill();

private:

	static KinectAdapter KinectAdapterState;	

	std::vector<_NUI_SKELETON_POSITION_INDEX> jointToRecord, humanGrowthPoints;
	SkeletJoints currentSkelet, rawCurrentSkelet;
	Surface16u savePoseDepth;
	int bufferDisconnect;
	int sleepSeconds;
	Timer sleepTimer;

	int		SKELET_SIZE;
	int		HEAD_INDEX;
	int		ANKLE_RIGHT_INDEX;
	int		ANKLE_LEFT_INDEX;
	float   SCREEN_HEIGHT;
	int     WRIST_RIGHT_INDEX;
	int     WRIST_LEFT_INDEX;

	void drawUserMask();
	void setActiveJoints();
};

inline KinectAdapter&	kinect() { return *KinectAdapter::Instance(); };