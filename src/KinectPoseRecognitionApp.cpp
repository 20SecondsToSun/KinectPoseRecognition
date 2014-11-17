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
		void			mouseUp( MouseEvent event );		
		void			update();
		void			draw();
		void			shutdown();

		void	plusBtnHandler();
		void	minusBtnHandler();
		void	closeBtnHandler();

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


		ButtonColor	  *plusBtn, *minusBtn, *closeBtn;
		ci::signals::connection plusBtnSignal, minusBtnSignal, closeBtnSignal;

		Texture *helpForRec, helpTex1, helpTex2, helpTex3, helpTex4;
};

void KinectPoseRecognitionApp::setup()
{
	setWindowSize(1920, 1080);
	setFrameRate(60);
	//setFullScreen(true);

	#ifndef debug
		setFullScreen(true);
		hideCursor();
	#endif

		
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Regular.ttf")), 55);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 46);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 26);
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 66);	
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 32);
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Bold.ttf")), 27 );
	fonts().loadFont( loadFile(getAssetPath("fonts/maestroc.ttf")), 114 );	
	fonts().loadFont( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 62);
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Bold.ttf")), 70 );
	fonts().loadFont( loadFile(getAssetPath("fonts/MyriadPro-Bold.ttf")), 26);

	//fonts().listFonts();

	hintFont = *fonts().getFont("Helvetica Neue", 46);
	state    = "ChooseMode";

	saver().loadConfigData();

    #ifndef recording
		bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );		
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
		setFullScreen(true);
		helpTex1 = loadImage(getAppPath()/"data/poses/test/Cat0.png");		
		helpTex2 = loadImage(getAppPath()/"data/poses/test/Cat1.png");		
		helpTex3 = loadImage(getAppPath()/"data"/"poses"/"test"/"Cat2.png");	
		helpTex4 = loadImage(getAppPath()/"data"/"poses"/"test"/"Cat3.png");		
		helpForRec = NULL;
		cameraCanon().setup();
		cameraCanon().live();
		kinect().setup();
		kinect().startTracking();
		poseNum = 0;
	#endif


    #ifndef recording
		recognitionGame().setup();
		game.init(getWindow());
		game.changeState(IntroScreen::Instance());
	#endif

	gl::enableAlphaBlending();


	#ifdef calibration
		setFullScreen(true);
		plusBtn = new ButtonColor(Rectf(10, 10, 110, 110), Color::hex(0xff0000), fonts().getFont("Helvetica Neue", 66), "+");
		plusBtnSignal = plusBtn->mouseDownEvent.connect(boost::bind(&KinectPoseRecognitionApp::plusBtnHandler, this));

		minusBtn = new ButtonColor(Rectf(150, 10, 260, 110), Color::hex(0xff0000), fonts().getFont("Helvetica Neue", 66), "-");
		minusBtnSignal = minusBtn->mouseDownEvent.connect(boost::bind(&KinectPoseRecognitionApp::minusBtnHandler, this));

		closeBtn = new ButtonColor(Rectf(1700, 10, 1860, 160), Color::hex(0xff0000), fonts().getFont("Helvetica Neue", 66), "exit");
		closeBtnSignal = closeBtn->mouseDownEvent.connect(boost::bind(&KinectPoseRecognitionApp::closeBtnHandler, this));
	#endif
}

void KinectPoseRecognitionApp::plusBtnHandler()
{
	int32_t angle = kinect().getTilt() + 1;
	kinect().setTilt(angle);
}

void KinectPoseRecognitionApp::minusBtnHandler()
{
	int32_t angle = kinect().getTilt() - 1;	
	kinect().setTilt(angle);
}

void KinectPoseRecognitionApp::closeBtnHandler()
{
	shutdown();
}

void KinectPoseRecognitionApp::changeState()
{
	
}

void KinectPoseRecognitionApp::update()
{	
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
				Utils::textFieldDraw("Для записи позы жми 1", &hintFont, Vec2f(400.f, getWindowHeight()*0.5f), colorZhint);
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
		
	#ifdef calibration	
		gl::enableAlphaBlending();
		plusBtn->draw();
		minusBtn->draw();
		closeBtn->draw();
	#endif

	#ifdef recording	
		if (helpForRec)
		{
			gl::pushMatrices();
			gl::translate(Vec2f(0.0f, (1080.0f - 1440.0f) * 0.5f));
			gl::draw(*helpForRec);
			gl::popMatrices();
		}
	#endif

	toucher().draw();
}

void KinectPoseRecognitionApp::mouseDown( MouseEvent event )
{
	toucher().setPosition(event.getPos());

	//#ifdef debug	
		recognitionGame().testPercent100 = true;
	//#endif
}

void KinectPoseRecognitionApp::mouseUp( MouseEvent event )
{
	// #ifdef debug	
		recognitionGame().testPercent100 = false;
	//#endif
}

void KinectPoseRecognitionApp::keyDown( KeyEvent event )
{	
//	console()<< "  event.getChar() "<<event.getChar()<<endl;

	console()<< "  event.getCode() "<<event.getCode()<<endl;

	if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) 
	{
		quit();
		return;
	}

	#ifdef calibration	
		if( event.getCode() == 61) 
			plusBtnHandler();
		else if( event.getCode() == 45) 
			minusBtnHandler();
	 #endif

	int32_t angle = 0;
	#ifdef recording	
	   switch (event.getCode())
		{ 		
			 #ifndef calibration
				case 49:
					state = "RecordingPose";			
					saveTimer.start();
				break;
				case 51:
					if (state == "SaveOrNot")	
					{		
						recognitionGame().saveAsTemplate(poseNum);
						poseNum++;
					}
					state = "ChooseMode";	
					kinect().startTracking();
				break;
				case 52:			
					state = "ChooseMode";	
					kinect().startTracking();
				break;

				case 113:
					helpForRec = &helpTex1;
				break;

				case 119:
					helpForRec = &helpTex2;
				break;

				case 101:
					helpForRec = &helpTex3;
				break;

				case 114:
					helpForRec = &helpTex4;
				break;
				case 116:
					helpForRec = NULL;
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
		}
	#endif
}

void KinectPoseRecognitionApp::shutdown()
{	
	#ifdef kinectUsed
	try
	{
		console()<<"try kinect shutdown"<<endl;
		kinect().Shutdown();
	}
	catch(...)
	{
		console()<<"kinect shutdown error!!!!"<<endl;
	}
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
		//ResultScreen::Instance()->shutdown();	
		//IntroScreen::Instance()->shutdown();
		//MainGameScreen::Instance()->shutdown();	
		socialPopup().shutdown();
	}
	catch(...)
	{
		console()<<"socialPopup shutdown error!!!!"<<endl;
	}

	#ifdef calibration
		closeBtnSignal.disconnect();
		minusBtnSignal.disconnect();
		plusBtnSignal.disconnect();
	#endif
}

#pragma warning(pop)
CINDER_APP_NATIVE( KinectPoseRecognitionApp, RendererGl )