#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
//#include "cinder/Text.h"
#include "cinder/Timer.h"
#include "cinder/Json.h"
#include "Utils.h"
#include "Kinect.h"
#include "Params.h"

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
	const float	  BOX_SCALE							= 250.f;
	static const std::string  DATA_BASE_NAME		= "poses.json";
	static const std::string  JSON_STORAGE_FOLDER	= "poses";
}

struct boxStruct 
{
	double x, y, z, w, h, d;
};
class Pose 
{
	public :
		void	setName(std::string _name){ name = _name;};
		void	setPoints(std::vector<ci::Vec3f> _rawPoints){ rawPoints = _rawPoints;};
		void	setOriginPoints(std::vector<ci::Vec3f> _rawPoints){ originPoints = _rawPoints;};
		
		std::string	getName()  { return name; };
		std::vector<ci::Vec3f>	getPoints(){ return rawPoints; };
		std::vector<ci::Vec3f>	getOriginPoints(){ return originPoints; };
		ci::gl::Texture	getImage()  { return _image; };
		void	setImage(ci::gl::Texture val)  { _image = val; };		

		void	drawPoints()
		{ 
			for (int j = 0; j < rawPoints.size(); j++)
			{
				gl::pushMatrices();			
				gl::drawSolidCircle( Vec2f(rawPoints[j].x, rawPoints[j].y), 3);	
				gl::popMatrices();
			}
		};
		void	drawLabel() {};

		void	setXYZ0(Vec3f _x0y0z0BoxPoint)
		{
			x0y0z0BoxPoint = _x0y0z0BoxPoint;
		};

		ci::Vec3f	getXYZ0()
		{
			return x0y0z0BoxPoint;
		};

	private:
		std::string				name;
		std::vector<ci::Vec3f>	rawPoints;
		std::vector<ci::Vec3f>	originPoints;		

		ci::Vec3f				x0y0z0BoxPoint;

		ci::gl::Texture		_image;

};

class KinectAdapter 
{
	public:
		boost::signals2::signal<void(void)>			levelCompleteEvent, gameOverEvent;

		void Setup();	
		void Shutdown();	

		void update();
		void draw();

		static KinectAdapter* Instance() {
			return &KinectAdapterState;
		}

		ci::gl::TextureRef getColorTexRef();
		ci::gl::TextureRef getDepthTexRef();
	
		Rectf				getColorResolutionRectf();
		
		float				getMinDepth(){ return kinectDefaults::MIN_DEPTH;};
		float				getMaxDepth(){ return kinectDefaults::MAX_DEPTH;};
		
		ci::gl::Texture		getColorTex();		
		ci::Surface8u	    getSurface8u();	

		void				handsSleep(int seconds);
		void				faceSleep(int seconds);

		void				drawKinectCameraColorSurface();
		void				calculateAspects();	
		float				viewShiftX, viewShiftY; 
		float				headScale;
		int					viewWidth, viewHeight;

		void				loadPoseBase();

		void				swapTracking()	;		
		void				stopTracking();
		void				startTracking();

		void				saveAsTemplate(std::string name);
		void				matchTemplate();
		boxStruct			boundingBox() ;

		void				savePoseBase();
		void				addPose(Pose* pose);

		std::vector<Pose*>  poses;		

		string						gameMode;

		void						startPoseGame();
		void						nextPose();

		std::string					getPoseIndex();

		int							getSkeletsInFrame();

		void						drawLoadedPoses();

		bool						isGameRunning, isPoseDetecting;

		void						drawPoseComics();

		void						poseComplete();

		float						getPoseProgress();


	protected:
		KinectAdapter() { }

	private:

		static KinectAdapter		KinectAdapterState;

		MsKinect::DeviceRef			mDevice;
		MsKinect::DeviceOptions     deviceOptions;
		MsKinect::Frame				mFrame;	

		ci::gl::TextureRef			mTextureColor;
		ci::gl::Texture				_mTextureColor;
		ci::gl::TextureRef			mTextureDepth;	

		ci::Channel16u				mDepthChannel16u;

		void						kinectConnect();
		void						updateSkeletonData();	

		ci::Surface8u				surface8u;	

		int							handsSleepSeconds, faceSleepSeconds;
		Timer						handsSleepTimer, faceSleepTimer;

		std::vector<ci::Vec3f>		currentSkelet, currentSkeletToDraw;
		
		bool						isTracking;
		
		ci::Font					hintFont;		
		Pose*						foundPose;

		void						preparePoseTemplate();
		void						scaleTo();
		void						translateTo();
		void						calculateCentroid();
		ci::Vec3f					centroid;

		float						mathPercent;


		void						setX0Y0Z0BoxPoint();
		ci::Vec3f					x0y0z0BoxPoint;

		void						drawUserMask();
		void						drawSkeletJoints();


		std::vector<_NUI_SKELETON_POSITION_INDEX>			jointToRecord;


		Surface16u					savePoseDepth;



		
		int							currentPoseInGame;


		int							levelCompletion;


};


inline KinectAdapter&	kinect() { return *KinectAdapter::Instance(); };

