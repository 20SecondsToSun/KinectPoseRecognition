#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "KinectAdapter.h"
#include "cinder/Timer.h"
#include "Utils.h"
#include "LocationEngine.h"
#include "IntroScreen.h"
#include "MainGameScreen.h"
#include "ResultScreen.h"
#include "cinder/params/Params.h"
#include "Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class KinectPoseRecognitionApp : public AppNative {
  public:
	void				setup();
	void				keyDown( KeyEvent event );
	void				update();
	void				draw();
	void				shutdown();

	static const int	secondsToRec		= 5;	

	 ci::Font				hintFont;
	 Timer				saveTimer;
	 string				state, poseName;

	private:
		LocationEngine			 game;
		params::InterfaceGlRef   mParams;
		void					changeState();
		
		
};

void KinectPoseRecognitionApp::setup()
{
	setWindowSize(1600, 1000);

	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 46);
	fonts().listFonts();

	hintFont = ci::Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 46 );
	state    = "ChooseMode";

	//KinectAdapter::Instance()->Setup();
	//KinectAdapter::Instance()->loadPoseBase();

   #ifndef recording
	
	IntroScreen::Instance()->setup();
	//MainGameScreen::Instance()->setup();
	ResultScreen::Instance()->setup();

	game.init("init", getWindow());
	game.changeState(IntroScreen::Instance());
	#endif


	#ifdef debug
		mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( Vec2i( 200, 400 ) ) );
		mParams->addParam( "boxMaxErrorX", &Params::boxMaxErrorX ).min( 5.f ).max( 50.5f ).step( 5.f );
		mParams->addParam( "boxMaxErrorY", &Params::boxMaxErrorY ).min( 5.f ).max( 50.5f ).step( 5.f );
		mParams->addParam( "maxErrorBetweenJoints", &Params::maxErrorBetweenJoints ).min( 10.f ).max( 200.0f ).step( 10.f );
		mParams->addParam( "percentForMatching", &Params::percentForMatching ).min( 0.1f ).max( 0.9f ).step( .1f );
		//mParams->addParam( "recordingMode", &Params::recording );			
	#endif

	gl::enableAlphaBlending();	
}

void KinectPoseRecognitionApp::changeState()
{
	
}

void KinectPoseRecognitionApp::update()
{
	 #ifdef recording	
		KinectAdapter::Instance()->gameMode = "RecordingPose";
		KinectAdapter::Instance()->update();

		 if (state == "RecordingPose")
		{
			if ( !saveTimer.isStopped() &&  saveTimer.getSeconds() > secondsToRec)
			{
				KinectAdapter::Instance()->stopTracking();
				saveTimer.stop();
				state = "SaveOrNot";
			}	
		}	
	#else	
		KinectAdapter::Instance()->gameMode = "MatchingGame";
		game.update();
	#endif
	
}

void KinectPoseRecognitionApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
	#ifdef recording	
		KinectAdapter::Instance()->draw();

		ColorA colorZhint = ColorA(1, 1, 1, 1);
		gl::disableAlphaBlending();

		if (state == "ChooseMode")
		{
			Utils::textFieldDraw("Для записи позы жми 1 \nДля теста игры жми 2", hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
		}
		else if (state == "RecordingPose")
		{
			Utils::textFieldDraw(to_string((int)saveTimer.getSeconds()) + "/" + to_string(secondsToRec), hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
		}
		else if (state == "SaveOrNot")
		{
			Utils::textFieldDraw("Для сохранения жми [3] \nНет - жми [4]", hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
		}
		gl::enableAlphaBlending();	
	#else
		game.draw();

	#endif	

	#ifdef debug
		mParams->draw();

	#endif
}

void KinectPoseRecognitionApp::keyDown( KeyEvent event )
{
	#ifdef recording	
	   switch (event.getChar())
		{ 
			case '1':
				state = "RecordingPose";			
				KinectAdapter::Instance()->gameMode = "RecordingPose";
				saveTimer.start();
			break;
			case '3':
				if (state == "SaveOrNot")	
				{					
					poseName = "Cat" + KinectAdapter::Instance()->getPoseIndex();					
					KinectAdapter::Instance()->saveAsTemplate(poseName);
				}
				state = "ChooseMode";	
				KinectAdapter::Instance()->startTracking();
			break;
			case '4':			
				state = "ChooseMode";	
				KinectAdapter::Instance()->startTracking();
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
				game.changeState(ResultScreen::Instance());
			break;
		}
	#endif
	
}

void KinectPoseRecognitionApp::shutdown( )
{
	//KinectAdapter::Instance()->Shutdown();
}

CINDER_APP_NATIVE( KinectPoseRecognitionApp, RendererGl )