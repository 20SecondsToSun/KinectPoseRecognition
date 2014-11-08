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
	bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );

	cameraCanon().setup();
	cameraCanon().live();

	kinect().setup();
	recognitionGame().setup();

	gameControls().setup();
	hintScreen().setup();
	comicsScreen().setup();

	/////////////////////////////////
	Texture comeBackBtnTex   = *AssetManager::getInstance()->getTexture( "images/diz/toStart.png" );
		
	comeBackBtn = new ButtonTex(comeBackBtnTex,  "backtoStart");
	comeBackBtn->setScreenField(Vec2f(0.0f, 958.0f));
	comeBackBtn->setDownState(comeBackBtnTex);
}

void MainGameScreen::init( LocationEngine* game)
{		
	_game = game;	

	isLeaveAnimation = false;
	deviceError = false;

	comeBackBtnSignal	   = comeBackBtn->mouseDownEvent.connect(boost::bind(&MainGameScreen::gotoFirstScreen, this));

	cameraCanon().reset();
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

	photoFlashSignal	  = recognitionGame().photoFlashEvent.connect(boost::bind(&MainGameScreen::photoFlashHandler, this));	

	gameControls().init();
	hintScreen().init();
	recognitionGame().initnew();
}

void MainGameScreen::cameraConnectedHandler() 
{
	gotoFirstScreen();
}

void MainGameScreen::gotoFirstScreen() 
{	
	if(!isLeaveAnimation && cameraCanon().isConnected && kinect().isConnected())
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
	cameraCanon().update();
	kinect().update();

	if(!deviceError)
		deviceError = !cameraCanon().isConnected || !kinect().isConnected();
	
	if(deviceError == false)	
		recognitionGame().update();
	//else
}

void MainGameScreen::draw() 
{
	gl::enableAlphaBlending();		

	if(deviceError == false)	
	{
		drawGame();
	}
	else
	{	
		drawDeviceError();
	}

	drawFadeOutIfAllow();
}

void MainGameScreen::drawDeviceError() 
{	
	string errorDeviceMessage ="";

	if (cameraCanon().isConnected == false)
	{
		errorDeviceMessage = "ÊÀÌÅÐÀ ÂÛÊËÞ×ÅÍÀ";

		recognitionGame().stopAllTimersIfNeed();	
		if(!_missedTimer.isStopped()) 
				_missedTimer.stop();		
	}
	
	if (kinect().isConnected() == false)
	{
		if(errorDeviceMessage.size() == 0)
		{
			errorDeviceMessage = "ÊÈÍÅÊÒ ÂÛÊËÞ×ÅÍ";
		}
		else
		{
			errorDeviceMessage += " | ÊÈÍÅÊÒ ÂÛÊËÞ×ÅÍ";
		}

		recognitionGame().stopAllTimersIfNeed();
		if(!_missedTimer.isStopped()) 
				_missedTimer.stop();		
	}

	Texture errorTexure = Utils::getTextField(errorDeviceMessage, fonts().getFont("MaestroC", 114),  Color::white());	
	gl::draw(errorTexure, Vec2f(0.5f*(1920.0f - errorTexure.getWidth()), 348.0f));
}

void MainGameScreen::drawGame() 
{
	cameraCanon().draw();

	#ifdef debug
		kinect().draw();
	#endif // debug


	switch(recognitionGame().state)
	{
		case STEP_BACK_MESSAGE:
			hintScreen().draw();
			comeBackBtn->draw();
		break;
		
		case HINT_MESSAGE:			
			gameControls().draw();
			hintScreen().draw();
		break;

		case PRE_GAME_INTRO:
		case COUNTER_STATE:
			hintScreen().draw();		
		break;

		case COUNTERS_ANIMATE:
			gameControls().draw();
			hintScreen().draw();
		break;

		case MAIN_GAME:			
			gameControls().draw();
			hintScreen().draw();

			if (Params::isPointsDraw)
				recognitionGame().drawJoints();
			
		break;

		case SHOW_GAME_RESULT:				
			comicsScreen().draw();
			gameControls().draw();
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

void MainGameScreen::drawPoseComics()
{
	gl::pushMatrices();
		gl::translate(cameraCanon().getSurfaceTranslate());
		gl::scale(-cameraCanon().scaleFactor, cameraCanon().scaleFactor);
		gl::draw(recognitionGame().getCurrentScreenShot());
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(getWindowWidth() - 360.0f, getWindowHeight() - 512.0f);		
		gl::draw(recognitionGame().getPoseImage());
	gl::popMatrices();
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
		//gl::color(ColorA(BLUE_FADE.r, BLUE_FADE.g, BLUE_FADE.b, alphaFinAnimate));	
		//gl::drawSolidRect(getWindowBounds());
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
}
#pragma warning(pop)