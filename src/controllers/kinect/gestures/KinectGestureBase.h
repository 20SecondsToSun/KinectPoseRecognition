#pragma once
#include "Kinect.h"

namespace KinectGesture
{
    class KinectGestureBase
    {
		public:
			boost::signals2::signal<void(void)> gestureRecognizedEvent;
    };
}