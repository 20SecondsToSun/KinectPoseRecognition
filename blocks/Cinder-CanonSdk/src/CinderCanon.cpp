/*
 * CinderCanon.cpp
 *
 * Created by Red Paper Heart
 *
 * Copyright (c) 2012, Red Paper Heart, All rights reserved.
 * This code is intended for use with the Cinder C++ library: http://libcinder.org
 *
 * To contact Red Paper Heart, email hello@redpaperheart.com or tweet @redpaperhearts
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "CinderCanon.h"
#include "cinder/Filesystem.h"

EdsError EDSCALLBACK DownloadImageProgress(EdsUInt32	inPercent
                                           ,EdsVoid*	inContext
                                           ,EdsBool*	outCancel)
{
	printf("Canon: downloading image: %d.\n", (int)inPercent);
	return EDS_ERR_OK;
};

using namespace canon;

static PhotoHandler * SingletonPhotoHandler = NULL;

CinderCanon::CinderCanon()  : bCameraIsConnected( false )
                            , bIsLiveView( false )
{

}

bool CinderCanon::setup(PhotoHandler * photoHandler, int cameraIndex )
{
	 SingletonPhotoHandler = photoHandler;

    // Initialize sdk
    EdsError err = EdsInitializeSDK();
    if( err != EDS_ERR_OK )
	{ 
		//console() << "Cinder-Canon :: Couldn't initialize SDK" << endl; 
		//return false;	
	}
    
    // Get Camera List and open a session owith one of them
    err = EdsGetCameraList( &mCamera_list );

    if( err != EDS_ERR_OK )
	{ 
		console() << "Cinder-Canon :: Couldn't retrieve camera list" << endl; 
		return false;
	}
    
   // console() << "Cinder-Canon :: initialized.  Found " << getNumConnectedCameras() << " camera(s) connected" << endl;
    
    if( getNumConnectedCameras() == 0 ) return false;

	
	 err = EdsGetChildAtIndex(mCamera_list, cameraIndex, &mCamera );

	if( err != EDS_ERR_OK )
	{ 
		console() << "Cinder-Canon :: Couldn't retrieve camera from list" << endl; 
		return false;
	}

	getDeviceInfo( mCamera );
	
	try
	{ 
		err = EdsOpenSession( mCamera );
	}
	catch(...)
	{
		console() << "TOO FAST SWITCH!!!!!!!!!!!!!!!" << endl;
		return false;
	}
	
	if( err != EDS_ERR_OK )
	{ 
		console() << "Cinder-Canon :: Couldn't open camera session" << endl;
		return false;
	}
		 
	// Set event listener
	EdsSetObjectEventHandler(mCamera, kEdsObjectEvent_All, CinderCanon::handleObjectEvent, (EdsVoid*)this );
	EdsSetPropertyEventHandler(mCamera, kEdsPropertyEvent_All, CinderCanon::handlePropertyEvent, (EdsVoid*)this );
	EdsSetCameraStateEventHandler(mCamera, kEdsStateEvent_All, CinderCanon::handleStateEvent, (EdsVoid*)this );

	EdsImageQuality quality = EdsImageQuality_S2JF;
	err = EdsSetPropertyData(mCamera, kEdsPropID_ImageQuality, 0, sizeof(quality), &quality);
	console()<<"image quality set: "<<quality<<"  "<<CanonErrorToString(err)<<"    "<<endl;
   
	bCameraIsConnected = true;
	bFrameNew = false;
	mLivePixels = Surface8u( 1024, 680, false, SurfaceChannelOrder::RGB ); // what about this resolution?


	bIsBusy = false;

	return true;
}

void CinderCanon::getDeviceInfo( EdsCameraRef cam )
{
    EdsError err = EDS_ERR_OK;
    
    EdsDeviceInfo info;
    err = EdsGetDeviceInfo( cam, &info );
    if( err != EDS_ERR_OK )
	{ 
		console() << "Cinder-Canon :: Couldn't retrieve camera info" << endl; 
	}
    
    console() << "Cinder-Canon :: Device name :: " << info.szDeviceDescription << endl;
	console() << "Cinder-Canon :: deviceSubType :: " << info.deviceSubType << endl;
	console() << "Cinder-Canon :: reserved :: " << info.reserved << endl;
	console() << "Cinder-Canon :: szPortName :: " << info.szPortName << endl;
}

int CinderCanon::getNumConnectedCameras()
{
    EdsUInt32 numCameras = 0;
    EdsGetChildCount( mCamera_list, &numCameras );
    
    return numCameras;
}

void CinderCanon::takePicture(PhotoHandler * photoHandler)
{
    if( !bCameraIsConnected )
        return;
    
    SingletonPhotoHandler = photoHandler;
    
   // console() << "Cinder-Canon :: Attempting to take picture" << endl;
    EdsError err;

 	EdsInt32 saveTarget = kEdsSaveTo_Host;
	err = EdsSetPropertyData( mCamera, kEdsPropID_SaveTo, 0, 4, &saveTarget );
 	
	EdsCapacity newCapacity = {0x7FFFFFFF, 0x1000, 1};
	err = EdsSetCapacity(mCamera, newCapacity);

	err = sendCommand( mCamera, kEdsCameraCommand_TakePicture, 0 );

	// console() << "Cinder-Canon ::kEdsCameraCommand_TakePicture " << CanonErrorToString(err)<<endl;
	 
	 if(err != EDS_ERR_OK) 
	 {
		console() << "Cinder-Canon :: take picture error :: " <<err<< endl;//36097
		SingletonPhotoHandler->photoCameraError(err);
	 }
	 else
	 {
		 console() << "Cinder-Canon :: take picture :: OK :: " << endl;//36097
	 }
}

EdsError CinderCanon::startLiveView()
{

	if( bIsLiveView )
		return EDS_ERR_OK;
		
	 //EdsUInt32 Oneshot = 0;
	 //EdsGetPropertyData(mCamera, kEdsPropID_AFMode, 0, sizeof(Oneshot), &Oneshot);
	 //console() << "Cinder-Canon :: start live view" <<Oneshot<< endl;

	EdsError err = EDS_ERR_OK;     
    // Get the output device for the live view image
    EdsUInt32 device;
    err = EdsGetPropertyData(mCamera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device );
    
    // PC live view starts by setting the PC as the output device for the live view image.
    if(err == EDS_ERR_OK)
    {
        device |= kEdsEvfOutputDevice_PC;
        err = EdsSetPropertyData(mCamera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
		if (err == EDS_ERR_OK)
		{
		  bIsLiveView = true;
		  bIsBusy = false;
		}
		else
		{
			//startLiveView();
			//shutdown();
		} 
    }
	else
	{
		console() << "Cinder-Canon :: start live view ERROR" << endl;
	}
	if (err == EDS_ERR_DEVICE_BUSY)
		bIsBusy = true;
	
    return err;
    // A property change event notification is issued from the camera if property settings are made successfully.
    // Start downloading of the live view image once the property change notification arrives.
}

void CinderCanon::endLiveView()
{
	if( !bIsLiveView ) return;

    EdsError err = EDS_ERR_OK;
	
    // Get the output device for the live view image
    EdsUInt32 device;
    err = EdsGetPropertyData(mCamera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device );
  
    // PC live view ends if the PC is disconnected from the live view image output device.
    if(err == EDS_ERR_OK)
    {
        device &= ~kEdsEvfOutputDevice_PC;
        err = EdsSetPropertyData(mCamera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
    }
    
    bIsLiveView = false;
}

void CinderCanon::extendShutDownTimer()
{
	try 
	{
		// always causes EDS_ERR_DEVICE_BUSY, even with live view disabled or a delay
		// but if it's not here, then the camera shuts down after 5 minutes.
		console()<<"try to extended timer !!!!"<<endl;
		auto err =sendCommand(mCamera, kEdsCameraCommand_ExtendShutDownTimer, 0);

		if(err == EDS_ERR_OK)
		{
			console()<<"NO SHUT OFF!!!!"<<endl;
		}	
		else
			console()<<"((((((((("<<endl;
	} 
	catch (...) 
	{
		console() << "Error while sending kEdsCameraCommand_ExtendShutDownTimer with Eds::SendStatusCommand: " << endl;
	}
}


EdsError CinderCanon::downloadEvfData( EdsCameraRef camera )
{
    if( !bIsLiveView ){
        console() << "No live view" << endl;
        startLiveView();
        return EDS_ERR_OK;
    }
   // console() << "Cinder-Canon :: downloadEvfData=============================== :: " << endl;


    EdsError err = EDS_ERR_OK;
    EdsStreamRef stream = NULL;
    EdsEvfImageRef evfImage = NULL;
    
    // Create memory stream.
    err = EdsCreateMemoryStream( 0, &stream);
    
    // Create EvfImageRef.
    if(err == EDS_ERR_OK) {
        err = EdsCreateEvfImageRef(stream, &evfImage);
    }
    
    // Download live view image data.
    if(err == EDS_ERR_OK){
        err = EdsDownloadEvfImage(camera, evfImage);
    }
    
    // Get the incidental data of the image.
    if(err == EDS_ERR_OK){
        // Get the zoom ratio
        EdsUInt32 zoom;
        EdsGetPropertyData(evfImage, kEdsPropID_Evf_ZoomPosition, 0, sizeof(zoom), &zoom );
        
        // Get the focus and zoom border position
        EdsPoint point;
        EdsGetPropertyData(evfImage, kEdsPropID_Evf_ZoomPosition, 0 , sizeof(point), &point);
    }
    
    // Display image
    EdsUInt32 length;
    unsigned char* image_data;
    EdsGetLength( stream, &length );
    if( length <= 0 ) return EDS_ERR_OK;

    EdsGetPointer( stream, (EdsVoid**)&image_data );
    
    // reserve memory
    Buffer buffer( image_data, length );    
    mLivePixels = Surface( loadImage( DataSourceBuffer::create(buffer), ImageSource::Options(), "jpg" ) );
    bFrameNew = true;
    
    // Release stream
    if(stream != NULL) {
        EdsRelease(stream);
        stream = NULL;
    }
    // Release evfImage
    if(evfImage != NULL) {
        EdsRelease(evfImage);
        evfImage = NULL;
    }
    
    return EDS_ERR_OK;
}
string CinderCanon::getFileName()
{
	return photoFileName;
}

void CinderCanon::downloadImage(EdsDirectoryItemRef dirItem, PhotoHandler * photoHandler)
{
    EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;

	// console() <<"DOWNLOAD IMAGE-----------------------------------"<<std::endl;

	// Get info about new image.
	EdsDirectoryItemInfo dir_item_info;

	err = EdsGetDirectoryItemInfo(dirItem, &dir_item_info);
	if(err != EDS_ERR_OK) 
	{
		 console() <<"Canon: error while trying to get more info about image to be downloaded.\n"<<std::endl;
	}
	
	// Created file stream to download image.
    fs::path downloadPath = photoHandler->photoDownloadDirectory() / string(dir_item_info.szFileName);
	photoFileName = string(dir_item_info.szFileName);

	if(err == EDS_ERR_OK) {
		err = EdsCreateFileStream(downloadPath.generic_string().c_str(),
                                  kEdsFileCreateDisposition_CreateAlways,
                                  kEdsAccess_ReadWrite,
                                  &stream);
	}
	if(err != EDS_ERR_OK) {
		 console() <<"Canon: error while creating download stream.\n"<<std::endl;
	}
	
	// Set progress
	if(err == EDS_ERR_OK) {
		err = EdsSetProgressCallback(stream, DownloadImageProgress, kEdsProgressOption_Periodically, this);
	}
	if(err != EDS_ERR_OK) {
		 console() << "Canon: error while setting download progress function.\n"<<std::endl;
	}
	
	// Download image.
	if(err == EDS_ERR_OK) {
		err = EdsDownload(dirItem, dir_item_info.size, stream);
	}
	if(err != EDS_ERR_OK) {
		 console() << "Canon: error while downloading item.\n" << "\n";
	}
	//console() <<"DOWNLOAD IMAGE---------------------------------- 3-"<<std::endl;
	
	// Tell we're ready.
	if(err == EDS_ERR_OK)
	{        
        console() << "Downloaded image to --- " << downloadPath.generic_string() << "\n";
		err = EdsDownloadComplete(dirItem);		
	}
	else
	{
		console() << "Downloaded image cancled" << "\n";
		EdsDownloadCancel(dirItem);	
	}

	if(err != EDS_ERR_OK)
	{
		console() <<"Canon: error while telling we're ready with the file download.\n"<< "\n";
	}
	else
	{
		//console() << "Try to delete image " << "\n";
		//err = EdsDeleteDirectoryItem(dirItem);	
	}

	if(err != EDS_ERR_OK)
	{
		console() <<"Canon: error while erasing.\n"<< "\n";
	}

	
	
	// Release dir item.
    /*
	if(dirItem != NULL) {
		err = EdsRelease(dirItem);
		if(err != EDS_ERR_OK) {
			printf("Canon: error releasing dir item when downloading.\n");
		}
		dirItem = NULL;
	}
	*/
    
	// Release stream
	if(stream != NULL) {
		err = EdsRelease(stream);
		if(err != EDS_ERR_OK) {
			console()<<"Canon: error while releasing download stream.\n"<<std::endl;
		}
		stream = NULL;
	}
    
    if (err != EDS_ERR_OK)
    {
       // downloadDest = "";
    }

    if (photoHandler)
    {
        photoHandler->photoDownloaded(downloadPath.generic_string(), err);
    }
}

EdsError EDSCALLBACK CinderCanon::handleObjectEvent(EdsUInt32 inEvent,
                                                    EdsBaseRef inRef,
                                                    EdsVoid* inContext )
{
   console() << "Cinder-Canon :: Object Callback :: " << CanonEventToString(inEvent) <<"   "<<inEvent<< endl;
    switch(inEvent) {
		case kEdsObjectEvent_DirItemRequestTransfer:
        case kEdsObjectEvent_DirItemCreated:
        {
            if (SingletonPhotoHandler)
            {
                console() << "Photo Taken. Calling photoTaken" << std::endl;
                EdsDirectoryItemRef dirItem = (EdsDirectoryItemRef)inRef;
                // NOTE: This is only called on success.
                // It should also be called on failure.
                SingletonPhotoHandler->photoTaken(dirItem, EDS_ERR_OK);
            }
            else
            {
                console() << "No photo callback. Ignoring." << endl;
                // This downloads to /tmp/canon
                // ((CinderCanon *)inContext)->downloadImage(inRef, NULL);
            }
        	break;
		}
	}
    
    if(inRef != NULL) {
        EdsRelease(inRef);
    }

    return EDS_ERR_OK;
}

EdsError EDSCALLBACK CinderCanon::handlePropertyEvent( EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext )
{
   // console() << "Cinder-Canon :: Property Callback :: " << CanonPropertyToString(inPropertyID) << endl;
    
    if( inPropertyID == kEdsPropID_Evf_OutputDevice )
	{
       // console() << "Cinder-Canon :: ready for live viewing" << endl;
		if (SingletonPhotoHandler)         
		{
			SingletonPhotoHandler->photoCameraReadyLiveView();
		}
    }
    
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK CinderCanon::handleStateEvent(
                                                 EdsUInt32 inEvent
                                                 ,EdsUInt32 inParam
                                                 ,EdsVoid* inContext
                                                 )
{
	//console()<<"Cinder-Canon: handleStateEvent "<< CanonEventToString(inEvent)<<std::endl;

	switch(inEvent) {
            // Connection with camera lost (maybe power off?)
		case kEdsStateEvent_Shutdown:
		{
//			shutdown();
			if (SingletonPhotoHandler)         
			{
				SingletonPhotoHandler->handleStateEvent(inEvent);
			}
			break;
		}

		case kEdsStateEvent_WillSoonShutDown: 
		{
			if (SingletonPhotoHandler)         
			{
				SingletonPhotoHandler->handleStateEvent(inEvent);
			}
			break;
		}
		default:break;
	};

    return EDS_ERR_OK;
}

EdsError CinderCanon::sendCommand( EdsCameraRef inCameraRef, EdsUInt32 inCommand, EdsUInt32 inParam )
{
    EdsError err = EDS_ERR_OK;
    
    err = EdsSendCommand( inCameraRef, inCommand, inParam );
	//console() << "Cinder-Canon :: answer while sending command " <<  CanonErrorToString(err) << "." << endl;
    
    if(err != EDS_ERR_OK)
	{
		//console() << "Cinder-Canon :: error while sending command " <<  CanonErrorToString(err) << "." << endl;
		if(err == EDS_ERR_DEVICE_BUSY) {
			//return false;
		}
	}
    
    return err;
}

void CinderCanon::shutdown()
{ 
    EdsError err = EdsCloseSession( mCamera );
	console() << "Cinder-Canon :: answer shutdown " <<  CanonErrorToString(err) << "." << endl;      
    err = EdsTerminateSDK();
	console() << "EdsTerminateSDK " <<CanonErrorToString(err)<<endl;   
}