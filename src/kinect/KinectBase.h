#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "Utils.h"
#include "Kinect.h"
#include "Params.h"
#include "Pose.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace kinectDefaults
{
	const NUI_IMAGE_RESOLUTION COLOR_RESOLUTION = MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_640x480;
	//const NUI_IMAGE_RESOLUTION COLOR_RESOLUTION = MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_1280x960;
	const NUI_IMAGE_RESOLUTION DEPTH_RESOLUTION = MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_640x480;
	//MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_1280x960
	const float   MIN_DEPTH							= 0.8f;
	const float	  MAX_DEPTH							= 3.0f;	
}

class KinectBase
{
	public:
		void setDevice();
		void kinectConnect();
		void Shutdown();

		boost::signals2::signal<void(void)> kinectConnectedEvent;
		boost::signals2::signal<void(void)> kinectMissPersonEvent;
		boost::signals2::signal<void(void)> kinectFindPersonEvent;
	
		MsKinect::DeviceRef		mDevice;
		MsKinect::DeviceOptions deviceOptions;
		MsKinect::Frame			mFrame;	

		long frameID, lastFrameId;

		void	swapTracking()	;		
		void	stopTracking();
		void	startTracking();

		ci::gl::TextureRef	getColorTexRef();
		ci::gl::TextureRef	getDepthTexRef();
		ci::gl::Texture getColorTex();		
		ci::Surface8u getSurface8u();		
		ci::Channel16u getDepthChannel16u();
			
		Rectf	getColorResolutionRectf();
		
		float	getMinDepth(){ return kinectDefaults::MIN_DEPTH;};
		float	getMaxDepth(){ return kinectDefaults::MAX_DEPTH;};
	
		void	drawKinectCameraColorSurface();
		void	calculateAspects();	

		void	setTilt(int32_t angleInDegrees);
		int32_t getTilt();

		bool	isConnected();

		float	headScale;
		int		viewWidth, viewHeight;
		float	viewShiftX, viewShiftY; 
		bool	isTracking;			
		
		ci::Timer	reconnectTimer;

	protected:
		bool	_isConnected;

	private:		
		int32_t	currentKinectTiltInDegrees;	
		
};