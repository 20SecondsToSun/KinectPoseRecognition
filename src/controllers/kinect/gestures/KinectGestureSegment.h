#pragma once
#include "Kinect.h"

namespace KinectGesture
{
	enum GesturePartResult
    {
        Failed,
        Succeeded
    };

    class KinectGestureSegment
    {
		public:
			virtual GesturePartResult update(MsKinect::Skeleton skeleton) = 0;
    };
}