#pragma once
#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "cinder/gl/GlslProg.h"

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
		void drawDesaturate();
		void update();
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
	
	protected:
		CameraAdapter() { }
	
	private:
		static CameraAdapter		CameraAdapterState;

		ci::gl::GlslProg		mShader, mShaderColor;
		fs::path                    mPhotoDownloadFolder;

		Timer restartLiveViewTimer, reconnectTimer;
		bool isConnected;
		string  userPhotoFileName, pathToDownloadedPhoto;	

		Surface8u lastFrame;
		
		 std::string photoDownloadDirectory();
		 std::string photoCameraReadyLiveView();

		 double			viewShiftX, viewShiftY, scaleFactor;
		 bool			isAspectsCompute;

		
		/////////////////////////

		 CanonCamera  mCamera;	
		
};
inline CameraAdapter&	cameraCanon() { return *CameraAdapter::Instance(); };