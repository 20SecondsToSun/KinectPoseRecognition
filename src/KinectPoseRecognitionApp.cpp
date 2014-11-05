#pragma warning(push)
#pragma warning(disable: 4244)

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "LocationEngine.h"
#include "IntroScreen.h"
#include "MainGameScreen.h"
#include "ResultScreen.h"
#include "KinectAdapter.h"

#include "Params.h"
#include "Saver.h"
#include "Utils.h"
#include "PlayerData.h"

#include "Toucher.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class KinectPoseRecognitionApp : public AppNative
{
  public:	
		void			setup();
		void			keyDown( KeyEvent event );
		void			mouseDown( MouseEvent event );
		void			update();
		void			draw();
		void			shutdown();

		static const int secondsToRec = 7;	
		 
		ci::Font		hintFont;
		Timer			saveTimer;
		string			state;

		int poseNum;

		gl::Texture	bg;

	private:
		LocationEngine	game;
		params::InterfaceGlRef	mParams;
		void	changeState();
};

void KinectPoseRecognitionApp::setup()
{
	setWindowSize(1920, 1080);
	setFrameRate(60);

	#ifndef debug
		setFullScreen(true);
		hideCursor();
	#endif

	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 46);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 26);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 66);	
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 32);
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Bold.ttf")), 27 );
	fonts().loadFont( loadFile(getAssetPath("fonts/maestroc.ttf")), 114 );	
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 62);
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Bold.ttf")), 70 );
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Bold.ttf")), 26);
	fonts().listFonts();

	hintFont = *fonts().getFont("Helvetica Neue", 46);
	state    = "ChooseMode";	

    #ifndef recording
		bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );
		saver().loadConfigData();
		IntroScreen::Instance()->setup();
		MainGameScreen::Instance()->setup();
		ResultScreen::Instance()->setup();
	#endif

	#ifdef paramsDraw
	//	PlayerData::score = 3;
		mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( Vec2i( 500, 400 ) ) );
		mParams->addParam( "boxMaxErrorX", &Params::boxMaxErrorX ).min( 1.f ).max( 50.5f ).step( 1.f );
		mParams->addParam( "boxMaxErrorY", &Params::boxMaxErrorY ).min( 1.f ).max( 50.5f ).step( 1.f );
		mParams->addParam( "maxErrorBetweenJoints", &Params::maxErrorBetweenJoints ).min( 1.f ).max( 20.0f ).step( 1.f );
		mParams->addParam( "percentForMatching", &Params::percentForMatching ).min( 0.1f ).max( 0.9f ).step( .1f );
		mParams->addSeparator();
		mParams->addParam( "isConnected", &Params::isNetConnected );
		mParams->addParam( "Player Score", &PlayerData::score ).min( 0 ).max( 3 ).step( 1 );
		mParams->addSeparator();
		mParams->addParam( "Imitate_PhotofromDirError", &Params::photoFromDirError );
		mParams->addSeparator();
		mParams->addParam( "Imitate_netConnectionCheckError", &Params::serverConnectionCheckError );
		mParams->addParam( "Imitate_netConnectionCheckTimeout", &Params::serverConnectionCheckTimeout );
		mParams->addSeparator();
		mParams->addParam( "Imitate_serverPhotoLoadError", &Params::serverPhotoLoadError );
		mParams->addParam( "Imitate_serverPhotoLoadTimeout", &Params::serverPhotoLoadTimeout );		
		mParams->addSeparator();
		mParams->addParam( "Imitate_serverEmailSendError", &Params::serverEmailSendError );
		mParams->addParam( "Imitate_serverEmailSendTimeout", &Params::serverEmailSendTimeout );
		mParams->addSeparator();		
	#endif


	#ifdef recording	
		cameraCanon().setup();
		cameraCanon().live();
		kinect().setup();
		kinect().startTracking();
	#endif


    #ifndef recording
		recognitionGame().setup();
		game.init(getWindow());
		game.changeState(IntroScreen::Instance());
	#endif

	poseNum = 0;
	gl::enableAlphaBlending();
}

void KinectPoseRecognitionApp::changeState()
{
	
}

void KinectPoseRecognitionApp::update()
{
	cameraCanon().update();
	 #ifdef recording	
		kinect().update();
		kinect().updateSkeletonData();
		

		 if (state == "RecordingPose")
		{
			if ( !saveTimer.isStopped() &&  saveTimer.getSeconds() > secondsToRec)
			{
				kinect().stopTracking();
				saveTimer.stop();
				state = "SaveOrNot";
			}	
		}	
	#else	
		game.update();
	#endif	
}

void KinectPoseRecognitionApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	
	#ifdef recording
	    cameraCanon().draw();
		kinect().draw();

		ColorA colorZhint = ColorA(1, 1, 1, 1);
		gl::disableAlphaBlending();

		#ifndef calibration

			if (state == "ChooseMode")
			{
				Utils::textFieldDraw("Для записи позы жми 1 \nДля теста игры жми 2", &hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
			}
			else if (state == "RecordingPose")
			{
				Utils::textFieldDraw(to_string((int)saveTimer.getSeconds()) + "/" + to_string(secondsToRec), &hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
			}
			else if (state == "SaveOrNot")
			{
				Utils::textFieldDraw("Для сохранения жми [3] \nНет - жми [4]", &hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
			}
			gl::enableAlphaBlending();	
		#endif	

	#else
		gl::draw(bg);
		game.draw();
	#endif	

	#ifdef paramsDraw
		mParams->draw();
	#endif
	toucher().draw();
}

void KinectPoseRecognitionApp::mouseDown( MouseEvent event )
{
	toucher().setPosition(event.getPos());
}

void KinectPoseRecognitionApp::keyDown( KeyEvent event )
{	
	console()<< "   event.getChar() "<<event.getChar()<<endl;
	int32_t angle = 0;
	#ifdef recording	
	   switch (event.getChar())
		{ 
			case '0':
				angle = kinect().getTilt() + 1;
				kinect().setTilt(angle);
			break;
			case '9':
				angle = kinect().getTilt() - 1;
				kinect().setTilt(angle);
			break;
			case 'q':			
				shutdown();			
			break;
			 #ifndef calibration
				case '1':
					state = "RecordingPose";			
					saveTimer.start();
				break;
				case '3':
					if (state == "SaveOrNot")	
					{		
						recognitionGame().saveAsTemplate(poseNum);
						poseNum++;
					}
					state = "ChooseMode";	
					kinect().startTracking();
				break;
				case '4':			
					state = "ChooseMode";	
					kinect().startTracking();
				break;
			 #endif
		}
	#else
		switch (event.getChar())
		{ 
			case '1':
				game.changeState(IntroScreen::Instance());
			break;
			case '2':
				game.changeState(MainGameScreen::Instance());
			break;
			case '3':
				game.changeState(ResultScreen::Instance());
			break;
			case 'q':
				shutdown();
			break;
			case '0':
			#ifdef kinectUsed
				kinect().Shutdown();
			#endif
			break;
		}
	#endif
}

void KinectPoseRecognitionApp::shutdown()
{
	console()<<"SHUTDOWN!!!!"<<endl;

	#ifdef kinectUsed
		kinect().Shutdown();
	#endif
	try
	{
		cameraCanon().shutdown();
	}
	catch(...)
	{
		console()<<"cameraCanon shutdown error!!!!"<<endl;
	}

	try
	{
		ResultScreen::Instance()->shutdown();	
	}
	catch(...)
	{
		console()<<"ResultScreen::Instance()->shutdown()error!!!!"<<endl;
	}
		
	quit();
	//IntroScreen::Instance()->shutdown();
	//MainGameScreen::Instance()->shutdown();	
}
#pragma warning(pop)
CINDER_APP_NATIVE( KinectPoseRecognitionApp, RendererGl )