#include "MainGameScreen.h"

using namespace ci;
using namespace ci::app;
using namespace MainGameDefaults;

MainGameScreen MainGameScreen::MainGameScreenState;
//KinectAdapter* MainGameScreen::kinect	= KinectAdapter::Instance();

void MainGameScreen::setup()
{	
	debugFont26 = *fonts().getFont("Helvetica Neue", 26);
	debugFont46 = *fonts().getFont("Helvetica Neue", 66);

	stateMemoMap[SHOW_FIRST_MESSAGE] = "Отойдите подальше";
	stateMemoMap[PRE_GAME_INTRO]	 = "Приготовьтесь к котопозе";
	stateMemoMap[MAIN_GAME]			 = "Играем";
	stateMemoMap[SHOW_GAME_RESULT]	 = "Результат кривляний";
	stateMemoMap[NONE]				 = "Пустое состояние";

	failImage 	= *AssetManager::getInstance()->getTexture( "images/fail.jpg" );

	comics1_godzilla = *AssetManager::getInstance()->getTexture( "images/comics1/godzilla.png" );	
	comics2_cat = *AssetManager::getInstance()->getTexture( "images/comics2/crazy.jpg" );
	comics3_uni = *AssetManager::getInstance()->getTexture( "images/comics3/unicorn.png" );

	cameraCanon().live();
}

void MainGameScreen::init( LocationEngine* game)
{		
	_game = game;

	state = SHOW_FIRST_MESSAGE;

	peopleCount = 1;
	distanceToPlayer = 0.0f;
	currentPose = 1;

	PlayerData::initData();
	_preWaitingTimer.start();

	cameraCanon().userPhotoIsDownloaded		= false;
	cameraCanon().tryToTakePhoto				= false;
	cameraCanon().photoCameraErrorMsg			= "NONE";
}

void MainGameScreen::shutdown()
{
	
}

void MainGameScreen::cleanup()
{	
	_preWaitingTimer.stop();
	_preGameTimer.stop();
	_onePoseTimer.stop();
	_resultTimer.stop();
	_missedTimer.stop();
}

void MainGameScreen::pause()
{
	
}

void MainGameScreen::resume()
{
	
}

void MainGameScreen::mouseEvents( )
{
	MouseEvent event = _game->getMouseEvent();	

	if(!_game->isAnimationRunning() && event.isLeftDown()) 
	{

	}
}

void MainGameScreen::handleEvents()
{

}

void MainGameScreen::keyEvents()
{	
	KeyEvent _event = _game->getKeyEvent();

	 switch (_event.getChar())
    {       
		case '1':
			peopleCount = 1;
		break;

		case '0':
			peopleCount = 0;
		break;
	 }
}

void MainGameScreen::update() 
{
	//checkPersonMissed();
	cameraCanon().update();	
	
	switch(state)
	{
		case SHOW_FIRST_MESSAGE:
			if (preWaitingTimerIsFinished() && personisFound())
			{
				_preWaitingTimer.stop();
				_preGameTimer.start();
				state = PRE_GAME_INTRO;				
			}
		break;

		case PRE_GAME_INTRO:
			if (preGameTimerIsFinished())
			{
				_preGameTimer.stop();
				_onePoseTimer.start();
				kinect().isPoseDetecting = false;
				kinect().isGameRunning   = true;
				state = MAIN_GAME;
			}
		break;

		case MAIN_GAME:
			
			if (mainTimerIsFinished())
			{				
				stopPersonChecking();
				kinect().isPoseDetecting = false;
			}
			else
				checkPersonPose();
		break;

		case SHOW_GAME_RESULT:
			if(showGameResultTimeIsFinished())
			{
				_resultTimer.stop();

				if (allPoseDone())
				{
					nextPose();
					gotoResultScreen();
					state = NONE;
				}
				else
				{
					nextPose();
					_preGameTimer.start();
					state = PRE_GAME_INTRO;
				}
			}
		 break;
	}	
}

void MainGameScreen::checkPersonMissed() 
{
	peopleCount = kinect().getSkeletsInFrame();

	if (peopleCount == 0)
	{
		if (_missedTimer.isStopped())
			_missedTimer.start();
		else if (_missedTimer.getSeconds() > MAX_MISSED_TIME)
		{
			gotoIntroState();
		}
	}
	else if (!_missedTimer.isStopped())
			_missedTimer.stop();
}

void MainGameScreen::gotoIntroState() 
{
	_game->changeState(IntroScreen::Instance());
}

bool MainGameScreen::personisFound() 
{
	return peopleCount !=0;
}

bool MainGameScreen::preWaitingTimerIsFinished() 
{
	return  (!_preWaitingTimer.isStopped() &&  _preWaitingTimer.getSeconds() > PREWAITING_TIME);
}

bool MainGameScreen::preGameTimerIsFinished() 
{	
	return  (!_preGameTimer.isStopped()	  &&  _preGameTimer.getSeconds() > PREGAME_TIME);
}

bool MainGameScreen::mainTimerIsFinished() 
{
	return  (!_onePoseTimer.isStopped() &&  _onePoseTimer.getSeconds() > ONE_POSE_TIME);
}

bool MainGameScreen::showGameResultTimeIsFinished() 
{
	return  (!_resultTimer.isStopped() &&  _resultTimer.getSeconds() > RESULT_TIME);
}

void MainGameScreen::checkPersonPose() 
{
	kinect().update();

	if ( kinect().getPoseProgress() >= MATCHING_MAX_VALUE)
	{
		kinect().poseComplete();
		stopPersonChecking();

		Surface surface(kinect().getColorTex());

		PlayerData::score++;
		PlayerData::screenshot[currentPose-1]  = surface;
		PlayerData::isSuccess[currentPose-1]	 = true;
		//PlayerData::storyCode[currentPose]	 = 				
	}
}

void MainGameScreen::stopPersonChecking() 
{
	_onePoseTimer.stop();
	_resultTimer.start();
	kinect().isGameRunning = false;
	state = SHOW_GAME_RESULT;
}

bool MainGameScreen::allPoseDone() 
{
	return currentPose >= ALL_POSES_COUNT;
}

void MainGameScreen::gotoResultScreen() 
{
	_game->changeState(ResultScreen::Instance());
}

void MainGameScreen::nextPose() 
{
	currentPose++;
	kinect().nextPose();
}

void MainGameScreen::draw() 
{
	gl::enableAlphaBlending();	
	
	//cameraCanon().draw();

	drawCameraImage();
	

	gl::enableAlphaBlending();
	switch(state)
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
	}	
	
	#ifdef debug
		//drawDebugMessage();
	#endif	

	gl::disableAlphaBlending();
}

void MainGameScreen::drawCameraImage()
{
	kinect().drawKinectCameraColorSurface();
}

void MainGameScreen::drawFirstMessageBox()
{
	Utils::textFieldDraw("ОТОЙДИТЕ ПОДАЛЬШЕ", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void MainGameScreen::drawPreReadyCounterBox()
{
	gl::disableAlphaBlending();
	Utils::textFieldDraw("ПРИГОТОВЬТЕСЬ К ПОЗЕ "+to_string(currentPose)+" : "+to_string(PREGAME_TIME - (int)_preGameTimer.getSeconds()), &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	gl::enableAlphaBlending();
}

void MainGameScreen::drawPoseSilhouette()
{
	kinect().drawLoadedPoses();
	kinect().drawSkeletJoints();
	gl::disableAlphaBlending();
	gl::color(Color::white());
	Utils::textFieldDraw("ДО КОНЦА ПОПЫТКИ ОСТАЛОСЬ  "+to_string(ONE_POSE_TIME - (int)_onePoseTimer.getSeconds()), &debugFont26, Vec2f(1300.f, 10.0f), ColorA(1.f, 0.f, 0.f, 1.f));

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
		Utils::textFieldDraw("ПОЗЫ СОВПАЛИ УРА!!! ( "+to_string(RESULT_TIME - (int)_resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));
		gl::enableAlphaBlending();
		
	}
	else
	{
		gl::draw(failImage, centeredRect);
		gl::disableAlphaBlending();
		Utils::textFieldDraw("СТАРАЙТЕСЬ ЛУЧШЕ ( "+to_string(RESULT_TIME - (int)_resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));	
		gl::enableAlphaBlending();
	}
}

void MainGameScreen::drawPoseComics()
{
	gl::pushMatrices();
	gl::translate(kinect().viewShiftX, kinect().viewShiftY);	
	gl::scale(kinect().headScale, kinect().headScale);
	gl::draw(PlayerData::screenshot[currentPose-1]);
	gl::popMatrices();

	if (currentPose == 1)
	{
		gl::pushMatrices();

		gl::translate(getWindowWidth() - 360.0, getWindowHeight() - 512.0);		
		gl::draw(comics1_godzilla);
		gl::popMatrices();
	}
	else if (currentPose == 2)
	{
		gl::pushMatrices();
		gl::translate(getWindowWidth() - comics2_cat.getWidth(), getWindowHeight() - comics2_cat.getHeight());
		gl::draw(comics2_cat);
		gl::popMatrices();

	}
	else if (currentPose == 3)
	{
		gl::pushMatrices();
		gl::translate(getWindowWidth() - comics3_uni.getWidth(), getWindowHeight() - comics3_uni.getHeight());
		gl::draw(comics3_uni);
		gl::popMatrices();
	}
}

void MainGameScreen::drawDebugMessage()
{
	gl::color(Color::white());
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

	Utils::textFieldDraw(outString3, &debugFont26, Vec2f(10.f, 100.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}