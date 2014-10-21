#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "Utils.h"
#include "KinectBase.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

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
		void connect();
		void updateSkeletonData();

		std::vector<ci::Vec3f> getCurrentSkelet()
		{
			return currentSkelet;
		};

	private:

		static KinectAdapter KinectAdapterState;	

		std::vector<ci::Vec3f> currentSkelet;		
		void drawUserMask();		
		Surface16u savePoseDepth;		
		void setActiveJoints();		
		int bufferDisconnect;
		std::vector<_NUI_SKELETON_POSITION_INDEX>			jointToRecord;
};

inline KinectAdapter&	kinect() { return *KinectAdapter::Instance(); };