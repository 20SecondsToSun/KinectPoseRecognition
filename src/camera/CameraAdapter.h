#pragma once
#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

#include "CinderCanon.h"
#include "CanonCamera.h"

#include "Params.h"

using namespace std;

class CameraAdapter :public canon::PhotoHandler 
{
	public:	
		void loadResource();
		void setup();
		
		void draw();	
		void update();
		void reset();
		void reconnect();

		string getpathToDownloadedPhoto();
		string getpathToErrorPhoto();

		bool checkIfDownloaded();
		bool checkIfError();

		float getWidth();
		float getHeight();
		void  shutdown();

		void live();
		void liveOff();		

		void takePhoto();

		 // Delegate callbacks from canon::PhotoHandler
		void photoTaken(EdsDirectoryItemRef directoryItem, EdsError error);
		void photoDownloaded(const std::string & downloadPath, EdsError error);
		void photoCameraError( EdsError err);
		void handleStateEvent(EdsUInt32 inEvent);

		bool tryToTakePhoto, userPhotoIsDownloaded;
		string photoCameraErrorMsg;

		static CameraAdapter* Instance() 
		{
			return &CameraAdapterState;
		}	

		bool		isConnected;

		boost::signals2::signal<void(void)>		cameraConnectedEvent;

		ci::Surface8u		getSurface();

		ci::Vec2f			getSurfaceTranslate();
		double				scaleFactor;
		ci::Vec2f			translateSurface;
	
	protected:
		CameraAdapter() { }
	
	private:
		static CameraAdapter		CameraAdapterState;

		fs::path                    mPhotoDownloadFolder;

		Timer restartLiveViewTimer, reconnectTimer;
		
		string  userPhotoFileName, pathToDownloadedPhoto;	

		Surface8u lastFrame;
		
		 std::string photoDownloadDirectory();
		 std::string photoCameraReadyLiveView();

		 double			viewShiftX, viewShiftY;
		 bool			isAspectsCompute;

		 void			takePhotoThread();
		 std::shared_ptr<std::thread>				tkphThread;


		 CanonCamera  mCamera;	
		
};
inline CameraAdapter&	cameraCanon() { return *CameraAdapter::Instance(); };