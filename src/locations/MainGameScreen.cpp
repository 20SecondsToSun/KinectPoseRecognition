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

	stateMemoMap[SHOW_FIRST_MESSAGE] = "Отойдите подальше";
	stateMemoMap[PRE_GAME_INTRO]	 = "Приготовьтесь к котопозе";
	stateMemoMap[MAIN_GAME]			 = "Играем";
	stateMemoMap[SHOW_GAME_RESULT]	 = "Результат кривляний";
	stateMemoMap[NONE]				 = "Пустое состояние";	

	cameraCanon().setup();
	cameraCanon().live();

	kinect().setup();

	comeBackBtn = new ButtonColor(Rectf(1200,300, 1600, 400), RED, &debugFont26, "BACK");
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
	
	recognitionGame().initnew();
}

void MainGameScreen::gotoFirstScreen() 
{	
	if(!isLeaveAnimation && cameraCanon().isConnected && kinect().isConnected())
	{		
		_game->freezeLocation = true;
		isLeaveAnimation = true;
		alphaFinAnimate = 0;
		removeTimers();
		removeHandlers();	

		timeline().apply( &alphaFinAnimate, 1.0f, 1.9f, EaseOutCubic() ).finishFn( [ & ]( )
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
		_game->freezeLocation = true;
		isLeaveAnimation = true;
		alphaFinAnimate = 0;

		removeTimers();
		removeHandlers();

		timeline().apply( &alphaFinAnimate, 0.0f, 1.0f, 1.9f, EaseOutCubic() ).finishFn( [ & ]( )
		{
			_game->freezeLocation = false;
			_game->changeState(ResultScreen::Instance());	
		});
	}		
}

void MainGameScreen::update() 
{
	cameraCanon().update();	
	kinect().update();

	if(deviceError)
	{
		
	}
	else if (!cameraCanon().isConnected || !kinect().isConnected())
	{
		deviceError = true;
	}
	else
		recognitionGame().update();	
	
}

void MainGameScreen::draw() 
{
	gl::enableAlphaBlending();	

	cameraCanon().draw();	

	if (!somethingWrongWithDevices() && !deviceError) 
	{	
		switch(recognitionGame().state)
		{
			case SHOW_FIRST_MESSAGE:			
				drawFirstMessageBox();
			break;

			case PRE_GAME_INTRO:
				drawPreReadyCounterBox();
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

	drawFadeOutIfAllow();
}

bool MainGameScreen::somethingWrongWithDevices() 
{
	bool deviceConnected = true;
	string errorDeviceMessage ="";

	if (cameraCanon().isConnected == false)
	{
		errorDeviceMessage = "КАМЕРА ВЫКЛЮЧЕНА";

		recognitionGame().stopAllTimersIfNeed();	
		_missedTimer.stop();
		deviceConnected = false;
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
		_missedTimer.stop();
		deviceConnected = false;
	}
	if (!deviceConnected)	
		Utils::textFieldDraw(errorDeviceMessage, &debugFont26, Vec2f(10.0f, 10.0f), ColorA(1.f, 1.f, 1.f, 1.f));

	return !deviceConnected;
}

void MainGameScreen::drawFirstMessageBox()
{
	Utils::textFieldDraw("ОТОЙДИТЕ ПОДАЛЬШЕ", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void MainGameScreen::drawPreReadyCounterBox()
{
	gl::disableAlphaBlending();
	Utils::textFieldDraw("ПРИГОТОВЬТЕСЬ К ПОЗЕ "+to_string(recognitionGame().currentPose)+" : "+to_string(recognitionGame().PREGAME_TIME - (int)recognitionGame()._preGameTimer.getSeconds()), &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	gl::enableAlphaBlending();
}

void MainGameScreen::drawPoseSilhouette()
{
	kinect().drawLoadedPoses();
	kinect().drawSkeletJoints();
	gl::disableAlphaBlending();
	gl::color(Color::white());
	Utils::textFieldDraw("ДО КОНЦА ПОПЫТКИ ОСТАЛОСЬ  "+to_string(recognitionGame().ONE_POSE_TIME - (int)recognitionGame()._onePoseTimer.getSeconds()), &debugFont26, Vec2f(1300.f, 10.0f), ColorA(1.f, 0.f, 0.f, 1.f));

	Utils::textFieldDraw("MATCHING  " + to_string((int)kinect().getMatchPercent())+" %", &debugFont46, Vec2f(1300.f, 100.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::enableAlphaBlending();

	gl::pushMatrices();	
	gl::translate(Vec2f(1300.f, 200.0f));
	gl::color(ColorA(1,1,1,1));
	gl::drawSolidRect(Rectf(0, 0, 300, 50));
	gl::color(ColorA(1,0,0,1));
	gl::scale(kinect().getPoseProgress()/100.0f, 1.0f);
	gl::drawSolidRect(Rectf(0, 0, 300, 50));
	gl::popMatrices();
}

void MainGameScreen::drawGameResult()
{
	Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );

	if (kinect().isPoseDetecting)
	{		
		drawPoseComics();
		gl::disableAlphaBlending();
		Utils::textFieldDraw("ПОЗЫ СОВПАЛИ УРА!!! ( "+to_string(recognitionGame().RESULT_TIME - (int)recognitionGame()._resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));
		gl::enableAlphaBlending();
		
	}
	else
	{
		gl::draw(failImage, centeredRect);
		gl::disableAlphaBlending();
		Utils::textFieldDraw("СТАРАЙТЕСЬ ЛУЧШЕ ( "+to_string(recognitionGame().RESULT_TIME - (int)recognitionGame()._resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));	
		gl::enableAlphaBlending();
	}
}

void MainGameScreen::drawGameResultWithoutTimer()
{
	Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );

	if (kinect().isPoseDetecting)
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
		gl::draw(PlayerData::playerData[recognitionGame().currentPose-1].screenshot);
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(getWindowWidth() - 360.0, getWindowHeight() - 512.0);		
		gl::draw(kinect().getPoseImage());
	gl::popMatrices();
}

void MainGameScreen::drawPhotoFlash()
{
	gl::color(ColorA(1, 1, 1, recognitionGame().alphaFlashAnim));
	gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
	gl::color(Color::white());
}

void MainGameScreen::drawDebugMessage()
{
	/*gl::color(Color::white());
	string outString1	 = "state : " + stateMemoMap[state];
	Utils::textFieldDraw(outString1, &debugFont26, Vec2f(10.f, 10.0f), ColorA(1.f, 1.f, 1.f, 1.f));

	string outString2	 = "Людей в кадре : " + to_string(peopleCount) +
						   " | Расстояние до ближайшего игрока : " +  to_string(distanceToPlayer)+
						   " | Текущая поза : " +  to_string(currentPose)+"/"+  to_string(ALL_POSES_COUNT);


	Utils::textFieldDraw(outString2, &debugFont26, Vec2f(10.f, 40.0f), ColorA(1.f, 1.f, 1.f, 1.f));

	string outString3	 = "ТАЙМЕРЫ ::::::: \n| Таймер чтобы отойти : " + to_string(_preWaitingTimer.getSeconds()) +
		" \n| Подготовительный таймер : "+ to_string((int)_preGameTimer.getSeconds()) +
		" \n| Таймер для одной позы "+  to_string((int)_onePoseTimer.getSeconds()) +
		" \n| Таймер показа результата "+  to_string((int)_resultTimer.getSeconds());

	Utils::textFieldDraw(outString3, &debugFont26, Vec2f(10.f, 100.0f), ColorA(1.f, 1.f, 1.f, 1.f));*/
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
	cameraConnectionSignal.disconnect();

	kinectConnectionSignal.disconnect();
	kinectMissPersonSignal.disconnect();
	kinectFindPersonSignal.disconnect();

	gotoResultScreenSignal.disconnect();
}