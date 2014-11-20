#include "CameraAdapter.h"

CameraAdapter CameraAdapter::CameraAdapterState;

void CameraAdapter::loadResource()
{
	mPhotoDownloadFolder = Params::getPhotosStorageDirectory();

    if (!fs::exists(mPhotoDownloadFolder))
	{
		fs::create_directories(mPhotoDownloadFolder);
	}
}

void CameraAdapter::setup()
{
	//console()<<"try setup -------------------->"<<endl;
	isConnected = false;    
	tryToTakePhoto = true;
	isPhotoMakingInThread = false;
	userPhotoIsDownloaded = false;
	photoCameraErrorMsg = "NONE";
	isAspectsCompute = false;

    isConnected = mCamera.setup(this);

	if (isConnected)
	{
	   mCamera.startLiveView();
	   calculateAspects();
	   cameraConnectedEvent();
	}
}

void CameraAdapter::reset()
{
	userPhotoIsDownloaded	= false;
	isPhotoMakingInThread    = false;
	photoCameraErrorMsg		= "NONE";
}

void CameraAdapter::update()
{
	if (isConnected)
	{
		mCamera.update();	

		if (!restartLiveViewTimer.isStopped()  && (int)restartLiveViewTimer.getSeconds() == 2)
		{		
			photoCameraReadyLiveView();		
		}
	}
	else
	{
		if (!reconnectTimer.isStopped() && (int) reconnectTimer.getSeconds() % 2 == 0)		
			reconnect();
		else reconnectTimer.start();
	}
}

void CameraAdapter::calculateAspects()
{
	if (!isAspectsCompute)
	{
		double aspect =  (double)mCamera.getLiveSurface().getWidth()/mCamera.getLiveSurface().getHeight();
		double viewHeight, viewWidth;

		if( getWindowWidth() / getWindowHeight() > aspect)			
		{
			viewHeight = getWindowHeight();
			viewWidth = int(viewHeight * aspect);	
			scaleFactor = viewHeight/ mCamera.getLiveSurface().getHeight();
		}
		else 
		{ 
			viewWidth = getWindowWidth();
			viewHeight = int(viewWidth / aspect);	
			scaleFactor  = viewWidth / mCamera.getLiveSurface().getWidth();
		}
	
		viewShiftX =float( 0.5 * (getWindowWidth()  - viewWidth));
		viewShiftY= float( 0.5 * (getWindowHeight() - viewHeight));

		console()<<"scaleFactor    "<<mCamera.getLiveSurface().getWidth()<<"  "<<mCamera.getLiveSurface().getHeight()<<endl;

		translateSurface = Vec2f(viewShiftX + mCamera.getLiveSurface().getWidth()*scaleFactor, viewShiftY);
	}
}

Vec2f CameraAdapter::getSurfaceTranslate()
{
	return translateSurface;
}

void CameraAdapter::takePhoto()
{
	tryToTakePhoto = true;
	userPhotoIsDownloaded = false;
	photoCameraErrorMsg = "NONE";

	mCamera.endLiveView();
	isPhotoMakingInThread = true;
	takePhotoThread();
	//tkphThread = std::shared_ptr< boost::thread>( new  boost::thread( bind( &CameraAdapter::takePhotoThread, this ) ) );	
}

void CameraAdapter::takePhotoThread()
{	
    mCamera.takePicture(this);
	isPhotoMakingInThread = false;
}

void CameraAdapter::live()
{
	if (!mCamera.isLiveViewing())
		photoCameraReadyLiveView();
}

void CameraAdapter::liveOff()
{
	if (mCamera.isLiveViewing())
		mCamera.endLiveView();
}

void CameraAdapter::reconnect()
{
	reconnectTimer.stop();
	setup();	
}

bool CameraAdapter::checkIfDownloaded()
{
	//console()<< "  --------------------- userPhotoIsDownloaded  -------------------------  "<<userPhotoIsDownloaded<<endl;
	if(isPhotoMakingInThread)
	{
		isPhotoMakingInThread = false;
		//tkphThread->join();
	}

	return userPhotoIsDownloaded;
}

bool CameraAdapter::checkIfError()
{
	return photoCameraErrorMsg != "NONE";
}

string CameraAdapter::getpathToErrorPhoto()
{
	return "ERROR.jpg";
}

string CameraAdapter::getpathToDownloadedPhoto()
{
	return userPhotoFileName;
}

void CameraAdapter::draw()
{
	if (!isConnected ) return;

	gl::pushMatrices();	
		if (mCamera.isLiveViewing())//tryToTakePhoto == false)	
		{	
			gl::translate(1920, -100);
			gl::scale(-1.8181818, 1.8181818);
			gl::color(Color::white());
			gl::draw( gl::Texture( mCamera.getLiveSurface() ));				
			lastFrame = mCamera.getLiveSurface();			
		}
		else 
		{
			gl::translate(translateSurface);
			gl::scale(-1.8181818, 1.8181818);
			gl::color(Color::white());
			gl::draw(lastFrame);
		}
	gl::popMatrices();
	//if (mCamera.isLiveViewing())//tryToTakePhoto == false)	
	//	gl::draw( gl::Texture( mCamera.getLiveSurface() ));	
}

Surface8u CameraAdapter::getSurface()
{
	return mCamera.getLiveSurface();	
}

float CameraAdapter::getWidth()
{
	return 1056.0f;
}

float CameraAdapter::getHeight()
{
	return 704.0f;
}

void CameraAdapter::shutdown()
{
	mCamera.endLiveView();
	isConnected = false;
	mCamera.shutdown();
}

void CameraAdapter::photoCameraError( EdsError err)
{
	//string temp = (c_str, strnlen(c_str, max_length));
	photoCameraErrorMsg = CanonErrorToString(err);
	console()<<" ==   photoCameraError  = "<<CanonErrorToString(err)<<std::endl;
}

void CameraAdapter::photoTaken(EdsDirectoryItemRef directoryItem, EdsError error)
{
	console()<<" ==   photoTaken  = "<<std::endl;
    mCamera.downloadImage(directoryItem, this);
}

void CameraAdapter::photoDownloaded(const std::string & downloadPath, EdsError error)
{
	photoCameraReadyLiveView();
	//console()<<" ==   downloadedImage  = "<<mCamera.getCannon().getFileName()<<std::endl;	
   userPhotoFileName = photoDownloadDirectory().string() +"\\"+ mCamera.getCannon().getFileName();
   userPhotoIsDownloaded = true;	
   
}

// Delegate method to tell the canon where to download the full-res image.
fs::path CameraAdapter::photoDownloadDirectory()
{
    return getAppPath()/"data"/"hiResPhotos";////mPhotoDownloadFolder.generic_string();
}

// Delegate method when camera ready
std::string CameraAdapter::photoCameraReadyLiveView()
{
	if (isConnected == false)	 return "ERROR";
	if (mCamera.isLiveViewing()) return "START";

	//console()<<"START LIVE VIEW!!!!!!"<<std::endl;//
	//console()<<"isBusy? "<<mCamera.isBusy()<<std::endl;

	if (mCamera.isBusy())
	{
		//restartLiveViewTimer.start();
		//return "ERROR";
	}

	EdsError err = mCamera.startLiveView();

	if (err != EDS_ERR_OK)
	{
		restartLiveViewTimer.start();
	}
	else
	{
		restartLiveViewTimer.stop();
	}
	//console()<<"isBusy? "<<mCamera.isBusy()<<std::endl;
	
	console()<<"START LIVE VIEW??  "<< CanonErrorToString(err)<<std::endl;

	return "START";
}
void CameraAdapter::handleStateEvent(EdsUInt32 inEvent)
{
	if (kEdsStateEvent_Shutdown == inEvent)
	{
		console()<<"mCamera SHUTDOWN "<<endl;
		mCamera.endLiveView();
		isConnected = false;
	}
	else if (kEdsStateEvent_WillSoonShutDown == inEvent)
	{
		console()<<"mCamera.isBusy() "<<mCamera.isBusy()<<endl;
		mCamera.extendShutDownTimer();
	}
}