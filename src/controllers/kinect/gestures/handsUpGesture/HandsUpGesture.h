#pragma once
#include "KinectGestureSegment.h"
#include "KinectGestureBase.h"
#include "cinder/app/AppNative.h"

using namespace ci;
using namespace ci::app;
namespace KinectGesture
{
	class HandsUpSegment1 : public KinectGestureSegment
	{
	public:
		GesturePartResult update(MsKinect::Skeleton skeleton)
		{
			auto wristLeft = skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT);
			auto wristRight = skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT);

			auto spine = skeleton.find(NUI_SKELETON_POSITION_SPINE);

			if ( wristLeft != skeleton.end() && wristRight!= skeleton.end() && spine!= skeleton.end())
			{
				if (wristLeft->second.getPosition().y < spine->second.getPosition().y
					&& wristRight->second.getPosition().y < spine->second.getPosition().y)
				{
					//console()<<" succes 1"<<std::endl;
					return Succeeded;
				}
			}

			return Failed;
		}
	};

	class HandsUpSegment2 : public KinectGestureSegment
	{
	public:
		GesturePartResult update(MsKinect::Skeleton skeleton)
		{
			auto wristLeft = skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT);
			auto wristRight = skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT);

			auto head = skeleton.find(NUI_SKELETON_POSITION_HEAD);

			if ( wristLeft != skeleton.end() && wristRight!= skeleton.end() && head!= skeleton.end())
			{
				if (wristLeft->second.getPosition().y > head->second.getPosition().y
					&& wristRight->second.getPosition().y > head->second.getPosition().y)
				{
					//console()<<" succes 2"<<std::endl;
					return Succeeded;
				}
			}

			return Failed;
		}
	};

	class  HandsUpGesture: public KinectGestureBase
	{
	public:

		HandsUpGesture()
		{		
			segments[0] = new HandsUpSegment1();
			segments[1] = new HandsUpSegment2();

			currentSegment = 0;
		}

		void update(MsKinect::Skeleton skeleton)
		{
			//console()<<"currentSegment   "<<currentSegment<<std::endl;
			GesturePartResult result = segments[currentSegment]->update(skeleton);

			if (result == Succeeded)
			{
				if (currentSegment < SEGMENTS_COUNT - 1)
				{
					currentSegment++;
					//frameCount = 0;
				}
				else
				{
					console()<<"RECOGNIZED!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
					gestureRecognizedEvent();
					reset();
				}	
			}
		/*	else if (result == Failed && frameCount == FRAMES_FOR_GESTURE)
			{
				reset();
			}
			else
			{
				frameCount++;
			}*/
		}

	private:

		static const int FRAMES_FOR_GESTURE = 60;
		static const int SEGMENTS_COUNT = 2;

		KinectGestureSegment* segments[SEGMENTS_COUNT];

		int currentSegment;
		int frameCount;

		void reset()
		{
			currentSegment = 0;
			frameCount = 0;
		}
	};
}