#include "CameraAdapter.h"

CameraAdapter CameraAdapter::CameraAdapterState;

void CameraAdapter::loadResource()
{
	mShader = gl::GlslProg( loadAsset( "shaders/passThru_vert.glsl"),  loadAsset( "shaders/shriner.frag") );		
	mShaderColor = gl::GlslProg( loadAsset( "shaders/passThru_vert_color.glsl"),  loadAsset( "shaders/shriner_color.frag") );	

	mPhotoDownloadFolder = Params::getPhotosStorageDirectory();
    if (!fs::exists(mPhotoDownloadFolder)) fs::create_directories(mPhotoDownloadFolder); 
}

void CameraAdapter::setup()
{
	isConnected = false;    
	tryToTakePhoto = true;
	userPhotoIsDownloaded = false;
	photoCameraErrorMsg = "NONE";
	isAspectsCompute = false;

    isConnected = mCamera.setup(this);

	if (isConnected)
	{
	   mCamera.startLiveView();
	   cameraConnectedEvent();
	}
}

void CameraAdapter::update()
{
	if (isConnected)
	{
		mCamera.update();	

		if (!restartLiveViewTimer.isStopped()  && (int)restartLiveViewTimer.getSeconds() == 1)
		{		
			photoCameraReadyLiveView();		
		}

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
				scaleFactor  = viewWidth/ mCamera.getLiveSurface().getWidth();
			}
	
			viewShiftX =float( 0.5 * (getWindowWidth()  - viewWidth));
			viewShiftY= float( 0.5 * (getWindowHeight() - viewHeight));

			translateSurface = Vec2f(viewShiftX + mCamera.getLiveSurface().getWidth()*scaleFactor, viewShiftY);
		}
	}
	else
	{
		if (!reconnectTimer.isStopped() && (int) reconnectTimer.getSeconds() % 2 == 0)		
			reconnect();
		else reconnectTimer.start();
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
    mCamera.takePicture(this);	
	//tkphThread = shared_ptr<thread>( new thread( bind( &CameraAdapter::takePhotoThread, this ) ) );	
	
}

void CameraAdapter::takePhotoThread()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	
	

	tkphThread->detach();
}

void CameraAdapter::live()
{
	if (mCamera.isLiveViewing()) return;

	photoCameraReadyLiveView();
}

void CameraAdapter::liveOff()
{
	if (!mCamera.isLiveViewing()) return;

	mCamera.endLiveView();
}

void CameraAdapter::reconnect()
{
	console()<<"try reconnect ......................"<<std::endl;
	reconnectTimer.stop();
	setup();	
}

bool CameraAdapter::checkIfDownloaded()
{
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
	//console()<<"camera.mCamera.isBusy"<<camera.mCamera.isBusy()<<std::endl;
	//return;
	if (!isConnected ) return;

	//mShaderColor.bind();
	//mShaderColor.uniform( "basetex", 2 );

	gl::pushMatrices();		

		if (mCamera.isLiveViewing())//tryToTakePhoto == false)	
		{	
			gl::translate(translateSurface);
			gl::scale(-scaleFactor, scaleFactor);
			gl::color(Color::white());
			gl::draw( gl::Texture( mCamera.getLiveSurface() ));				
			lastFrame = mCamera.getLiveSurface();			
		}
		else 
		{
			gl::translate(translateSurface);
			gl::scale(-scaleFactor, scaleFactor);
			gl::color(Color::white());
			gl::draw(lastFrame);
		}

	gl::popMatrices();
	//mShaderColor.unbind();		
}

Surface8u CameraAdapter::getSurface()
{
	return mCamera.getLiveSurface();	
}

void CameraAdapter::drawDesaturate()
{
	//return;
	if (!isConnected) return;

	mShader.bind();
	mShader.uniform( "basetex", 2 );
	if (mCamera.isLiveViewing())//tryToTakePhoto == false)	
	{
		gl::draw( gl::Texture( mCamera.getLiveSurface() ));		
		lastFrame = mCamera.getLiveSurface();
	}
	else 
		gl::draw(lastFrame);
	mShader.unbind();		
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
	 mCamera.shutdown();
}


//-----------------------------------------------------------------------------------

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
	//console()<<" ==   downloadedImage  = "<<mCamera.getCannon().getFileName()<<std::endl;	
   userPhotoFileName = mCamera.getCannon().getFileName();
   userPhotoIsDownloaded = true;	
}

// Delegate method to tell the canon where to download the full-res image.
std::string CameraAdapter::photoDownloadDirectory()
{
    return mPhotoDownloadFolder.generic_string();
}

// Delegate method when camera ready
std::string CameraAdapter::photoCameraReadyLiveView()
{
	if (isConnected == false)	 return "ERROR";
	if (mCamera.isLiveViewing()) return "START";

	//console()<<"START LIVE VIEW!!!!!!"<<std::endl;
//
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
	//console()<<"STSWITCH OFF??  "<< std::endl;

	if (kEdsStateEvent_Shutdown == inEvent)
	{
		mCamera.endLiveView();
		isConnected = false;
	}	
}
