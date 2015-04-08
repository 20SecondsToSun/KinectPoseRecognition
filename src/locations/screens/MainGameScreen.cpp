#pragma warning(push)
#pragma warning(disable: 4244)

#include "MainGameScreen.h"
#include "Game.h"

using namespace ci;
using namespace ci::app;
using namespace colorsParams;
using namespace gameStates;

MainGameScreen MainGameScreen::MainGameScreenState;

void MainGameScreen::setup()
{
	bg  = *AssetManager::getInstance()->getTexture("images/diz/bg.jpg");

	cameraCanon().setup();
	cameraCanon().live();
	kinect().setup();
	recognitionGame().setup();
	gameControls().setup();
	hintScreen().setup();
	comicsScreen().setup();

	/////////////////////////////////
	Texture comeBackBtnTex   = *AssetManager::getInstance()->getTexture("images/diz/toStart.png");
	comeBackBtn = new ButtonTex(comeBackBtnTex,  "backtoStart");
	comeBackBtn->setScreenField(Vec2f(0.0f, 958.0f));
	comeBackBtn->setDownState(comeBackBtnTex);
}

void MainGameScreen::init( LocationEngine* game)
{		
	_game = game;	

	isLeaveAnimation = false;
	deviceError = false;

	comeBackBtnSignal = comeBackBtn->mouseUpEvent.connect(boost::bind(&MainGameScreen::gotoFirstScreen, this));

	cameraCanon().reset();
	cameraCanon().live();

	cameraConnectionSignal = cameraCanon().cameraConnectedEvent.connect(boost::bind(&MainGameScreen::cameraConnectedHandler, this));

	kinectMissPersonSignal = kinect().kinectMissPersonEvent.connect(boost::bind(&MainGameScreen::kinectMissPersonHandler, this));
	kinectConnectionSignal = kinect().kinectConnectedEvent.connect(boost::bind(&MainGameScreen::gotoFirstScreen, this));
	kinectFindPersonSignal = kinect().kinectFindPersonEvent.connect( [ & ]( )
	{
		_missedTimer.stop();
	});	

	gotoResultScreenSignal = recognitionGame().gotoResultScreenEvent.connect( [ & ]( )
	{
		gotoResultScreen();
	});	

	gotoFirstScreenSignal = recognitionGame().gotoFirstScreenEvent.connect( [ & ]( )
	{
		gotoFirstScreen();
	});	

	photoFlashSignal = recognitionGame().photoFlashEvent.connect(boost::bind(&MainGameScreen::photoFlashHandler, this));

	isCameraUpdating = false;

	cameraStartUpdateSignal = recognitionGame().cameraStartUpdateEvent.connect( [ & ]( )
	{
		isCameraUpdating = true;
		cameraCanon().update();	
	});	

	cameraStopUpdateSignal = recognitionGame().cameraStopUpdateEvent.connect( [ & ]()
	{
		isCameraUpdating = false;
	});	

	gameControls().init();
	hintScreen().init();
	recognitionGame().initnew();

	Utils::printVideoMemoryInfo();
}

void MainGameScreen::cameraConnectedHandler() 
{
	gotoFirstScreen();
}

void MainGameScreen::gotoFirstScreen() 
{
#ifndef nocamera
	if(!isLeaveAnimation && cameraCanon().isConnected && kinect().isConnected())
#else
	if(!isLeaveAnimation && kinect().isConnected())
#endif	
	{
		animationLeaveLocationPrepare();
		alphaFinAnimate = 0.0f;
		timeline().apply( &alphaFinAnimate,0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( [ & ]( )
		{
			_game->freezeLocation = false;
			_game->changeState(IntroScreen::Instance());
		}).delay(0.5);
	}
}

void MainGameScreen::kinectMissPersonHandler() 
{
	if (kinect().getSkeletsInFrame() == 0)
	{
		if (_missedTimer.isStopped())
		{
			_missedTimer.start();
		}
		else if (_missedTimer.getSeconds() > MAX_MISSED_TIME)
		{
			gotoFirstScreen();
		}
	}
	else if (!_missedTimer.isStopped())
	{
		_missedTimer.stop();
	}
}

void MainGameScreen::gotoResultScreen() 
{
	if(!isLeaveAnimation)
	{
		animationLeaveLocationPrepare();

		timeline().apply( &alphaFinAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( [ & ]( )
		{
			_game->freezeLocation = false;
			_game->changeState(ResultScreen::Instance());	
		});
	}		
}

void MainGameScreen::photoFlashHandler() 
{
	alphaFlashAnim = 1.0f;
	timeline().apply(&alphaFlashAnim, 1.0f,  0.0f, 1.1f , EaseInCubic()).delay(0.4f).finishFn( [ & ]( )
	{
		recognitionGame().flashAnimationFinished();
	});	
}

void MainGameScreen::animationLeaveLocationPrepare() 
{
	_game->freezeLocation = true;
	isLeaveAnimation = true;
	removeTimers();
	removeHandlers();
}

void MainGameScreen::update() 
{
	if (isCameraUpdating || !cameraCanon().isConnected)
		cameraCanon().update();

	kinect().update();

	if(!deviceError)
#ifndef nocamera
		deviceError = !cameraCanon().isConnected || !kinect().isConnected();
#else
		deviceError = !kinect().isConnected();
#endif

	if(deviceError == false && fboError == false)	
		recognitionGame().update();
}

void MainGameScreen::draw() 
{
	gl::enableAlphaBlending();		

	if(deviceError == false && fboError == false)	
	{
		drawGame();
	}
	else if  (fboError)
	{
		drawError();
		comeBackBtn->draw();
	}
	else
	{	
		drawDeviceError();
	}

	drawFadeOutIfAllow();
}

void MainGameScreen::drawDeviceError() 
{	
	string errorDeviceMessage = "";

	if (cameraCanon().isConnected == false)
	{
		errorDeviceMessage = "КАМЕРА ВЫКЛЮЧЕНА";

		recognitionGame().stopAllTimersIfNeed();	
		if(!_missedTimer.isStopped()) 
			_missedTimer.stop();
	}

	if (kinect().isConnected() == false)
	{
		if(errorDeviceMessage.size() == 0)
		{
			errorDeviceMessage = "КИНЕКТ ВЫКЛЮЧЕН";
		}
		else
		{
			errorDeviceMessage += " | КИНЕКТ ВЫКЛЮЧЕН";
		}

		recognitionGame().stopAllTimersIfNeed();
		if(!_missedTimer.isStopped()) 
			_missedTimer.stop();		
	}

	Texture errorTexure = Utils::getTextField(errorDeviceMessage, fonts().getFont("MaestroC", 114),  Color::white());	
	gl::draw(errorTexure, Vec2f(0.5f * (1920.0f - errorTexure.getWidth()), 348.0f));
}

void MainGameScreen::drawGame() 
{
	if (isCameraUpdating)
		cameraCanon().draw();

	switch(recognitionGame().state)
	{
	case STEP_BACK_MESSAGE:
		hintScreen().draw();
		comeBackBtn->draw();
		if (Params::isPointsDraw)
			recognitionGame().drawCurrentPlayerJoints();
		break;

	case HINT_MESSAGE:			
		gameControls().draw();
		hintScreen().draw();
		break;

	case PRE_GAME_INTRO:
	case COUNTER_STATE:	
		hintScreen().draw();		
		break;

	case HANDS_UP_AWAITING:
		hintScreen().draw();	
		if (Params::isPointsDraw)
			recognitionGame().drawCurrentPlayerJoints();
		break;		

	case COUNTERS_ANIMATE:
		gameControls().draw();
		hintScreen().draw();
		break;

	case MAIN_GAME:			
		gameControls().draw();
		hintScreen().draw();

		if (Params::isPointsDraw)
		{
			recognitionGame().drawJoints();
			recognitionGame().drawCurrentPlayerJoints();
		}
		break;

	case SHOW_GAME_RESULT:
		comicsScreen().draw();
		//gameControls().draw();
		break;

	case MAKE_SCREENSHOOT:
		comicsScreen().draw();
		break;

	case PHOTO_MAKING_WAIT:	
		gameControls().draw();
		drawPhotoFlash();
		break;

	case WIN_ANIMATION_FINISH_WAIT:
		gameControls().draw();
		drawPhotoFlash();
		break;
	}
}

void MainGameScreen::drawError()
{
	Utils::textFieldDraw("Что-то пошло не так...\nЕсли ситуация повторится,\nперезапустите программу",  fonts().getFont("MaestroC", 114), Vec2f(510.f, 348.0f), Color::white());
}

void MainGameScreen::drawPhotoFlash()
{
	gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaFlashAnim));
	gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), getWindowHeight()));
	gl::color(Color::white());
}

void MainGameScreen::drawFadeOutIfAllow() 
{
	if (isLeaveAnimation)
	{
		gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaFinAnimate));
		gl::draw(bg);
		gl::color(Color::white());
	}
}

void MainGameScreen::cleanup()
{
	removeTimers();
	removeHandlers();
}

void MainGameScreen::removeTimers()
{
	recognitionGame().stopAllTimersIfNeed();
	_missedTimer.stop();
}

void MainGameScreen::removeHandlers()
{
	comeBackBtnSignal.disconnect();
	photoFlashSignal.disconnect();	
	cameraConnectionSignal.disconnect();
	kinectConnectionSignal.disconnect();
	kinectMissPersonSignal.disconnect();
	kinectFindPersonSignal.disconnect();
	gotoResultScreenSignal.disconnect();
	gotoFirstScreenSignal.disconnect();
	cameraStartUpdateSignal.disconnect();
	cameraStopUpdateSignal.disconnect();
}
#pragma warning(pop)