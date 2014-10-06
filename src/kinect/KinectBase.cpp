#include "KinectBase.h"

using namespace kinectDefaults;

void KinectBase::setDevice()
{
	deviceOptions.enableColor(true);
	deviceOptions.enableDepth(true);
	deviceOptions.setDepthResolution(DEPTH_RESOLUTION);
	deviceOptions.setColorResolution(COLOR_RESOLUTION);	
	deviceOptions.setSkeletonSelectionMode(MsKinect::SkeletonSelectionMode::SkeletonSelectionModeClosest1);

	mDevice = MsKinect::Device::create();	

	mDevice->connectEventHandler( [ & ]( MsKinect::Frame frame )
	{
		mFrame = frame;

		if ( frame.getColorSurface() ) 
		{
			mTextureColor = gl::Texture::create( frame.getColorSurface() );
			_mTextureColor = gl::Texture(frame.getColorSurface());
			surface8u =  frame.getColorSurface();
		} 
		if ( frame.getDepthChannel() ) 
		{
			mTextureDepth	 = gl::Texture::create( frame.getDepthChannel() );
			mDepthChannel16u = frame.getDepthChannel();
		}
	} );
}

void KinectBase::kinectConnect()
{
	try {
		mDevice->start(deviceOptions);
	} catch ( MsKinect::Device::ExcDeviceCreate ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcDeviceInit ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcDeviceInvalid ex ) {
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcGetCoordinateMapper ex ) {
	//	console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcOpenStreamColor ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcOpenStreamDepth ex ) {
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcStreamStart ex ) {
	//	console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcUserTrackingEnable ex ) {
	//	console() << ex.what() << endl;
	}
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
	

void KinectBase::Shutdown()
{	
	mDevice->stop();	
}

ci::gl::TextureRef KinectBase::getColorTexRef()
{
	return mTextureColor;
}

ci::gl::Texture KinectBase::getColorTex()
{
	return _mTextureColor;
}

ci::gl::TextureRef KinectBase::getDepthTexRef()
{
	return mTextureDepth;
}

ci::Surface8u  KinectBase::getSurface8u()
{
	return surface8u;
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
	}		
}	

void KinectBase::drawKinectCameraColorSurface()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize());

	Texture colorRef =  getColorTex();

	if (colorRef)
	{		
		gl::enable( GL_TEXTURE_2D );	

		gl::color( ColorAf::white() );
		
		gl::pushMatrices();
			gl::translate(viewShiftX, viewShiftY);
			gl::scale(headScale, headScale);
			gl::draw( colorRef);//, colorRef.getBounds(), Rectf(0, 0, float(viewWidth),float(viewHeight)));	
		gl::popMatrices();		

		gl::disable( GL_TEXTURE_2D );
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