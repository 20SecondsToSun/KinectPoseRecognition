#include "MainGameScreen.h"
#include "Game.h"

using namespace ci;
using namespace ci::app;
using namespace colorsParams;
using namespace gameStates;

MainGameScreen MainGameScreen::MainGameScreenState;

void MainGameScreen::setup()
{
	failImage   = *AssetManager::getInstance()->getTexture( "images/fail.jpg" );

	debugFont26 = *fonts().getFont("Helvetica Neue", 26);
	debugFont46 = *fonts().getFont("Helvetica Neue", 66);

	//stateMemoMap[SHOW_FIRST_MESSAGE] = "ŒÚÓÈ‰ËÚÂ ÔÓ‰‡Î¸¯Â";
	stateMemoMap[PRE_GAME_INTRO]	 = "œË„ÓÚÓ‚¸ÚÂÒ¸ Í ÍÓÚÓÔÓÁÂ";
	stateMemoMap[MAIN_GAME]			 = "»„‡ÂÏ";
	stateMemoMap[SHOW_GAME_RESULT]	 = "–ÂÁÛÎ¸Ú‡Ú ÍË‚ÎˇÌËÈ";
	stateMemoMap[NONE]				 = "œÛÒÚÓÂ ÒÓÒÚÓˇÌËÂ";	

	cameraCanon().setup();
	cameraCanon().live();

	kinect().setup();
	recognitionGame().setup();

	gameControls().setup();
	hintScreen().setup();

	comeBackBtn = new ButtonColor(Rectf(1520,980, 1920, 1080), RED, &debugFont26, "BACK");
}

void MainGameScreen::init( LocationEngine* game)
{		
	_game = game;	

	isLeaveAnimation = false;
	deviceError = false;	

	comeBackBtnSignal	   = comeBackBtn->mouseDownEvent.connect(boost::bind(&MainGameScreen::gotoFirstScreen, this));

	cameraCanon().reset();
	cameraConnectionSignal = cameraCanon().cameraConnectedEvent.connect(boost::bind(&MainGameScreen::gotoFirstScreen, this));

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

void MainGameScreen::gotoFirstScreen() 
{	
	if(!isLeaveAnimation && !deviceError)
	{
		animationLeaveLocationPrepare();

		timeline().apply( &alphaFinAnimate,0.0f, 1.0f, 1.9f, EaseOutCubic() ).finishFn( [ & ]( )
		{
			_game->freezeLocation = false;
			_game->changeState(IntroScreen::Instance());
		});
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

		timeline().apply( &alphaFinAnimate, 0.0f, 1.0f, 1.9f, EaseOutCubic() ).finishFn( [ & ]( )
		{
			_game->freezeLocation = false;
			_game->changeState(ResultScreen::Instance());	
		});
	}		
}

void MainGameScreen::photoFlashHandler() 
{
	timeline().apply(&alphaFlashAnim, 1.0f,  0.0f, 1.1f , EaseInCubic()).delay(0.4).finishFn( [ & ]( )
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

	deviceError = !cameraCanon().isConnected || !kinect().isConnected();

	if(deviceError == false)	
		recognitionGame().update();
}

void MainGameScreen::draw() 
{
	gl::enableAlphaBlending();		

	if (deviceError) 
	{
		drawDeviceError();
	}
	else
	{	
		drawGame();
	}

	drawFadeOutIfAllow();
}

void MainGameScreen::drawDeviceError() 
{	
	string errorDeviceMessage ="";

	if (cameraCanon().isConnected == false)
	{
		errorDeviceMessage = " ¿Ã≈–¿ ¬€ Àﬁ◊≈Õ¿";

		recognitionGame().stopAllTimersIfNeed();	
		_missedTimer.stop();		
	}

	if (kinect().isConnected() == false)
	{
		if(errorDeviceMessage.size() == 0)
		{
			errorDeviceMessage = " »Õ≈ “ ¬€ Àﬁ◊≈Õ";
		}
		else
		{
			errorDeviceMessage += " |  »Õ≈ “ ¬€ Àﬁ◊≈Õ";
		}

		recognitionGame().stopAllTimersIfNeed();
		_missedTimer.stop();		
	}
	
	Utils::textFieldDraw(errorDeviceMessage, &debugFont26, Vec2f(10.0f, 10.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void MainGameScreen::drawGame() 
{
	cameraCanon().draw();	

	switch(recognitionGame().state)
	{
		case STEP_BACK_MESSAGE:
			hintScreen().draw();		
		break;
		
		case HINT_MESSAGE:
			hintScreen().draw();
			gameControls().draw();
		break;

		case PRE_GAME_INTRO:
			drawPreReadyCounterBox();
			gameControls().draw();
		break;

		case MAIN_GAME:
			drawPoseSilhouette();
		break;

		case SHOW_GAME_RESULT:				
			drawGameResult();			
		break;

		case MAKE_SCREENSHOOT:
			drawGameResultWithoutTimer();
		break;

		case PHOTO_MAKING_WAIT:
			drawPoseSilhouette();
			drawPhotoFlash();
		break;

		case WIN_ANIMATION_FINISH_WAIT:
			drawPoseSilhouette();
			drawPhotoFlash();
		break;
	}	

	comeBackBtn->draw();
}

void MainGameScreen::drawPreReadyCounterBox()
{
	gl::disableAlphaBlending();
	Utils::textFieldDraw("œ–»√Œ“Œ¬‹“≈—‹   œŒ«≈ "+to_string(recognitionGame().level)+" : "+to_string(recognitionGame().PREGAME_TIME - (int)recognitionGame()._preGameTimer.getSeconds()), &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	gl::enableAlphaBlending();
}

void MainGameScreen::drawPoseSilhouette()
{
	gl::enableAlphaBlending();

	gl::pushMatrices();
		gl::translate(kinect().getTranslation());
		gl::scale(kinect().getScale());		
		recognitionGame().drawCurrentSilhouette();
	gl::popMatrices();

	kinect().drawSkeletJoints();

	gl::disableAlphaBlending();
	gl::color(Color::white());
	Utils::textFieldDraw("ƒŒ  ŒÕ÷¿ œŒœ€“ » Œ—“¿ÀŒ—‹  "+to_string(recognitionGame().ONE_POSE_TIME - (int)recognitionGame()._onePoseTimer.getSeconds()), &debugFont26, Vec2f(1300.f, 10.0f), ColorA(1.f, 0.f, 0.f, 1.f));

	Utils::textFieldDraw("MATCHING  " + to_string((int)recognitionGame().getMatchPercent())+" %", &debugFont46, Vec2f(1300.f, 100.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::enableAlphaBlending();

	gl::pushMatrices();	
	gl::translate(Vec2f(1300.f, 200.0f));
	gl::color(ColorA(1,1,1,1));
	gl::drawSolidRect(Rectf(0, 0, 300, 50));
	gl::color(ColorA(1,0,0,1));
	gl::scale(recognitionGame().getPoseProgress()/100.0f, 1.0f);
	gl::drawSolidRect(Rectf(0, 0, 300, 50));
	gl::popMatrices();
}

void MainGameScreen::drawGameResult()
{
	Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );

	if (recognitionGame().isPoseDetecting)
	{		
		drawPoseComics();
		gl::disableAlphaBlending();
		Utils::textFieldDraw("œŒ«€ —Œ¬œ¿À» ”–¿!!! ( "+to_string(recognitionGame().RESULT_TIME - (int)recognitionGame()._resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));
		gl::enableAlphaBlending();
		
	}
	else
	{
		gl::draw(failImage, centeredRect);
		gl::disableAlphaBlending();
		Utils::textFieldDraw("—“¿–¿…“≈—‹ À”◊ÿ≈ ( "+to_string(recognitionGame().RESULT_TIME - (int)recognitionGame()._resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));	
		gl::enableAlphaBlending();
	}
}

void MainGameScreen::drawGameResultWithoutTimer()
{
	Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );

	if (recognitionGame().isPoseDetecting)
	{		
		drawPoseComics();
	}
	else
	{
		gl::draw(failImage, centeredRect);	
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
		gl::translate(getWindowWidth() - 360.0, getWindowHeight() - 512.0);		
		gl::draw(recognitionGame().getPoseImage());
	gl::popMatrices();
}

void MainGameScreen::drawPhotoFlash()
{
	gl::color(ColorA(1, 1, 1, alphaFlashAnim));
	gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
	gl::color(Color::white());
}

void MainGameScreen::drawFadeOutIfAllow() 
{
	if (isLeaveAnimation)
	{
		gl::color(ColorA(0, 0, 0, alphaFinAnimate));	
		gl::drawSolidRect(getWindowBounds());
		gl::color(ColorA(0,0,0,1));
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