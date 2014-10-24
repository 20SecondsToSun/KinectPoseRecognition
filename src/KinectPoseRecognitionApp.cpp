#pragma warning(push)
#pragma warning(disable: 4244)

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"


#include "LocationEngine.h"
//#include "IntroScreen.h"
#include "MainGameScreen.h"
//#include "ResultScreen.h"
//#include "KinectAdapter.h"

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
	  void				setup();
		void			keyDown( KeyEvent event );
		void			mouseDown( MouseEvent event );
		void			update();
		void			draw();
		void			shutdown();

	static const int	secondsToRec		= 5;	

	 ci::Font			hintFont;
	 Timer				saveTimer;
	 string				state;

	 gl::Texture		bg;

	private:
		LocationEngine			 game;
		params::InterfaceGlRef   mParams;
		void					changeState();
};

void KinectPoseRecognitionApp::setup()
{
	setWindowSize(1920, 1080);
	setFrameRate(60);
	//setFullScreen(true);

	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 46);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 26);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 66);	
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 32);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 32);
	fonts().listFonts();

	hintFont = *fonts().getFont("Helvetica Neue", 46);
	state    = "ChooseMode";

	bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );

    #ifndef recording
	
		saver().loadConfigData();

		//IntroScreen::Instance()->setup();
		MainGameScreen::Instance()->setup();
		//ResultScreen::Instance()->setup();

		//popup().start(popupTypes::EMAIL);	

		game.init(getWindow());
		game.changeState(MainGameScreen::Instance());
	#endif


	#ifdef debug
		PlayerData::score = 3;
		mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( Vec2i( 500, 400 ) ) );
		mParams->addParam( "boxMaxErrorX", &Params::boxMaxErrorX ).min( 5.f ).max( 50.5f ).step( 5.f );
		mParams->addParam( "boxMaxErrorY", &Params::boxMaxErrorY ).min( 5.f ).max( 50.5f ).step( 5.f );
		mParams->addParam( "maxErrorBetweenJoints", &Params::maxErrorBetweenJoints ).min( 10.f ).max( 200.0f ).step( 10.f );
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
		//mParams->addParam( "kinect", &Params::serverEmailSendError );
		//mParams->addParam( "Imitate_serverEmailSendTimeout", &Params::serverEmailSendTimeout );
		//mParams->addParam( "recordingMode", &Params::recording );			
	#endif


	#ifdef recording	
		cameraCanon().setup();
		cameraCanon().live();
		kinect().setup();
	#endif

	gl::enableAlphaBlending();	
}

void KinectPoseRecognitionApp::changeState()
{
	
}

void KinectPoseRecognitionApp::update()
{
	 #ifdef recording	
		kinect().update();
		kinect().updateSkeletonData();
		cameraCanon().update();

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

	gl::draw(bg);
	
	#ifdef recording
	    cameraCanon().draw();
		kinect().draw();


		ColorA colorZhint = ColorA(1, 1, 1, 1);
		gl::disableAlphaBlending();

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
	#else
		game.draw();
		//popup().draw();
		//kinect().draw();

	#endif	

	#ifdef debug
		//mParams->draw();
	#endif

	toucher().draw();
}

void KinectPoseRecognitionApp::mouseDown( MouseEvent event )
{
	toucher().setPosition(event.getPos());
}

void KinectPoseRecognitionApp::keyDown( KeyEvent event )
{
	int32_t angle = 0;
	#ifdef recording	
	   switch (event.getChar())
		{ 
			case '1':
				state = "RecordingPose";			
				saveTimer.start();
			break;
			case '3':
				if (state == "SaveOrNot")	
				{		
					recognitionGame().saveAsTemplate();
				}
				state = "ChooseMode";	
				kinect().startTracking();
			break;
			case '4':			
				state = "ChooseMode";	
				kinect().startTracking();
			break;
			case '0':
				angle = kinect().getTilt() + 1;
				kinect().setTilt(angle);
			break;
			case '9':
				angle = kinect().getTilt() - 1;
				kinect().setTilt(angle);
			break;
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
				//game.changeState(ResultScreen::Instance());
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

void KinectPoseRecognitionApp::shutdown( )
{
	console()<<"SHUT DOWN!!!!!!!!!!"<<endl;
	#ifdef kinectUsed
		kinect().Shutdown();
	#endif

	//cameraCanon().shutdown();
		
	//IntroScreen::Instance()->shutdown();
	//MainGameScreen::Instance()->shutdown();
	//ResultScreen::Instance()->shutdown();
	
	console()<<"!!!!!!!!!!!!!!!!SHUT DOWN!!!!!!!!!!"<<endl;
}
#pragma warning(pop)
CINDER_APP_NATIVE( KinectPoseRecognitionApp, RendererGl )
