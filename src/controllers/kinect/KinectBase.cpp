#include "KinectBase.h"

using namespace kinectDefaults;

void KinectBase::setDevice()
{	
	deviceOptions.enableColor(true);
	deviceOptions.enableDepth(true);
	deviceOptions.setDepthResolution(DEPTH_RESOLUTION);
	deviceOptions.setColorResolution(COLOR_RESOLUTION);	
	//deviceOptions.setSkeletonSelectionMode(MsKinect::SkeletonSelectionMode::SkeletonSelectionModeClosest1);	

	deviceOptions.setSkeletonTransform(MsKinect::SkeletonTransform::TRANSFORM_SMOOTH);

	mDevice = MsKinect::Device::create();	
	mDevice->connectEventHandler( [ & ]( MsKinect::Frame frame )
	{
		mFrame = frame;
	} );

	_gestureEnable = false;
}

void KinectBase::kinectConnect()
{	
	try 
	{
		mDevice->start(deviceOptions);
		_isConnected = true;
	} 
	catch ( MsKinect::Device::ExcDeviceCreate ex )
	{
		console() << ex.what() << endl;
	} 
	catch ( MsKinect::Device::ExcDeviceInit ex ) 
	{
		console() << ex.what() << endl;
	} 
	catch ( MsKinect::Device::ExcDeviceInvalid ex )
	{
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcGetCoordinateMapper ex ) {
	//	console() << ex.what() << endl;
	}
	catch ( MsKinect::Device::ExcOpenStreamColor ex )
	{
		console() << ex.what() << endl;
	} 
	catch ( MsKinect::Device::ExcOpenStreamDepth ex )
	{
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcStreamStart ex ) {
	//	console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcUserTrackingEnable ex ) {
	//	console() << ex.what() << endl;

		/*
		 
		*/
	}
}

bool KinectBase::isConnected()
{	
	return _isConnected;
}

void KinectBase::setTilt(int32_t angleInDegrees)
{
	currentKinectTiltInDegrees = angleInDegrees;	
	mDevice->setTilt(currentKinectTiltInDegrees);
}

int32_t KinectBase::getTilt()
{
	return mDevice->getTilt();
}

ci::gl::TextureRef KinectBase::getColorTexRef()
{
	return gl::Texture::create( mFrame.getColorSurface());
}

ci::gl::Texture KinectBase::getColorTex()
{
	return  gl::Texture(mFrame.getColorSurface());
}

ci::gl::TextureRef KinectBase::getDepthTexRef()
{
	return gl::Texture::create( mFrame.getDepthChannel());
}

ci::Channel16u KinectBase::getDepthChannel16u()
{
	return  mFrame.getDepthChannel();
}

ci::Surface8u  KinectBase::getSurface8u()
{
	return mFrame.getColorSurface();
}

Rectf KinectBase::getColorResolutionRectf()
{
	switch (COLOR_RESOLUTION)
	{
		case MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_640x480 :
			return Rectf(0.0f, 0.0f, 640.0f,480.0f);
		break;

		case MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_1280x960 :
			return Rectf(0.0f, 0.0f, 1280.0f,960.0f);
		break;

		default:
			return Rectf(0.0f, 0.0f, 640.0f,480.0f);
		break;
	}		
}

void KinectBase::swapTracking()
{
	isTracking = !isTracking;
}

void KinectBase::stopTracking()
{
	isTracking = false;
}

void KinectBase::startTracking()
{
	isTracking = true;
}

void KinectBase::Shutdown()
{	
	mDevice->stop();	
}

void KinectBase::drawKinectCameraColorSurface()
{	
	if (mFrame.getColorSurface())
	{
		gl::pushMatrices();
			gl::translate(viewShiftX, viewShiftY);
			gl::scale(headScale, headScale);		
			gl::draw(mFrame.getColorSurface());
		gl::popMatrices();
	}
}

void KinectBase::calculateAspects()
{
	Rectf kinectResolutionR = getColorResolutionRectf();
	float aspect =  kinectResolutionR.getWidth()/kinectResolutionR.getHeight();
			
	if( getWindowWidth() / getWindowHeight() > aspect)			
	{
		viewHeight = getWindowHeight();
		viewWidth = int(viewHeight * aspect);	
		headScale = viewHeight/ kinectResolutionR.getHeight();
	}
	else 
	{ 
		viewWidth = getWindowWidth();
		viewHeight = int(viewWidth / aspect);	
		headScale  = viewWidth/ kinectResolutionR.getWidth();
	}
	
	viewShiftX =float( 0.5 * (getWindowWidth()  - viewWidth));
	viewShiftY= float( 0.5 * (getWindowHeight() - viewHeight));		
}

Vec2f KinectBase::getTranslation()
{
	return Vec2f(viewShiftX, viewShiftY);
}

Vec2f KinectBase::getScale()
{
	return Vec2f(headScale, headScale);
}