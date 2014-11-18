#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "KinectBase.h"
#include "Utils.h"
#include "HandsUpGesture.h"


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
namespace KinectGesture
{	
	class Gesture: public KinectBase
	{
	public:
		enum Type
		{
			TYPE_INVALID ,
			HANDS_UP,
		};
	};
}
class KinectAdapter: public KinectBase
{
public:

	void setup();
	void draw();

	static KinectAdapter* Instance()
	{
		return &KinectAdapterState;
	}

	int getSkeletsInFrame();
	void drawSkeletJoints();

	void update();
	void updateSkeletonData();
	void updateSkeletonData(Rectf trackZone);

	bool allHumanPointsInScreenRect();

	float distanceToSkelet();
	float calculateHumanGrowsScaleTo180sm();
	float getEtalonHeightInPixelsAccordingDepth();
	float userHeightInPixels();
	float userHeightRaw();

	void enbleGesture(KinectGesture::Gesture::Type type, bool enable = true) const;

	std::vector<ci::Vec3f> getCurrentSkelet()
	{
		return currentSkelet;
	};

	bool isHandsUp();
	bool isDistanceOk();

	ci::Surface16u getSilhouette();

	void sleep(int seconds);
	void sleepKill();

	void enbleGestures();
	void disableGestures();

	ci::signals::connection _handsUpGestureSignal;

	bool isHeapInRect( Rectf rect);

private:

	static KinectAdapter KinectAdapterState;	

	SkeletJoints currentSkelet, rawCurrentSkelet;		

	void drawUserMask();
	void setActiveJoints();

	Surface16u savePoseDepth;
	int bufferDisconnect;
	std::vector<_NUI_SKELETON_POSITION_INDEX> jointToRecord, humanGrowthPoints;

	int sleepSeconds;
	Timer sleepTimer;

	int		SKELET_SIZE;
	int		HEAD_INDEX;
	int		ANKLE_RIGHT_INDEX;
	int		ANKLE_LEFT_INDEX;
	float   SCREEN_HEIGHT;
	int     WRIST_RIGHT_INDEX;
	int     WRIST_LEFT_INDEX;

	KinectGesture::HandsUpGesture _handsUpGesture;
};

inline KinectAdapter&	kinect() { return *KinectAdapter::Instance(); };