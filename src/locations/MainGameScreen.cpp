#include "MainGameScreen.h"

using namespace ci;
using namespace ci::app;
using namespace MainGameDefaults;

MainGameScreen MainGameScreen::MainGameScreenState;

void MainGameScreen::setup()
{	
	debugFont26 = *fonts().getFont("Helvetica Neue", 26);
	debugFont46 = *fonts().getFont("Helvetica Neue", 66);

	stateMemoMap[SHOW_FIRST_MESSAGE] = "Îòîéäèòå ïîäàëüøå";
	stateMemoMap[PRE_GAME_INTRO]	 = "Ïðèãîòîâüòåñü ê êîòîïîçå";
	stateMemoMap[MAIN_GAME]			 = "Èãðàåì";
	stateMemoMap[SHOW_GAME_RESULT]	 = "Ðåçóëüòàò êðèâëÿíèé";
	stateMemoMap[NONE]				 = "Ïóñòîå ñîñòîÿíèå";

	failImage 		 = *AssetManager::getInstance()->getTexture( "images/fail.jpg" );

	cameraCanon().cameraConnectedEvent.connect( 
		boost::bind(&MainGameScreen::cameraIsConnectedNow, this) 
		);	

	comeBackBtn = new ButtonColor(getWindow(), Rectf(1200,300, 1600, 400), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"ÍÀÇÀÄ");
}

void MainGameScreen::cameraIsConnectedNow( )
{
	_game->changeState(IntroScreen::Instance());
}

void MainGameScreen::init( LocationEngine* game)
{		
	_game = game;
	state = SHOW_FIRST_MESSAGE;

	#ifdef debug
		peopleCount = 1;
	#else
		peopleCount = 0;
	#endif

	distanceToPlayer = 0.0f;


	kinect().startPoseGame();
	
	currentPose = 1;//
	poseCode = kinect().getPoseCode();

	PlayerData::initData();
	_preWaitingTimer.start();

	cameraCanon().userPhotoIsDownloaded			= false;	
	cameraCanon().photoCameraErrorMsg			= "NONE";

	comeBackBtn->mouseDownEvent.connect(boost::bind(&MainGameScreen::gotoFirstScreen, this));
	comeBackBtn->addEventListener(MouseEvents::MOUSE_DOWN);
}

void MainGameScreen::shutdown()
{
	
}

void MainGameScreen::cleanup()
{	
	stopAllTimersIfNeed();
	comeBackBtn->removeConnect(MouseEvents::MOUSE_DOWN);
}

void MainGameScreen::pause()
{
	
}

void MainGameScreen::resume()
{
	
}

void MainGameScreen::mouseEvents( )
{

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
	#ifndef debug
		//checkPersonMissed();	
	#endif

	cameraCanon().update();	

	if( !cameraCanon().isConnected)
		return;
		
	switch(state)
	{
		case SHOW_FIRST_MESSAGE:
			updateFirstMessage();
		break;

		case PRE_GAME_INTRO:
			updatePreGameIntro();
		break;

		case MAIN_GAME:			
			updateMainGame();
		break;

		case SHOW_GAME_RESULT:
			updateGameResult();
		 break;

		case PHOTO_MAKING_WAIT:		
			updatePhotoMaking();
		break;

		case WIN_ANIMATION_FINISH_WAIT:
			updateAnimationWait();
		break;

		case MAKE_SCREENSHOOT:			
			makeScreenShootUpdtae();
		break;
	}	
}

void MainGameScreen::updateFirstMessage() 
{
	if (preWaitingTimerIsFinished() )//&& personisFound())
	{
		_preWaitingTimer.stop();
		_preGameTimer.start();
		state = PRE_GAME_INTRO;				
	}
}

void MainGameScreen::updatePreGameIntro() 
{
	if (preGameTimerIsFinished())
	{
		_preGameTimer.stop();
		_onePoseTimer.start();
		kinect().isPoseDetecting = false;
		kinect().isGameRunning   = true;
		state = MAIN_GAME;
	}
}

void MainGameScreen::updateMainGame() 
{
	if (mainTimerIsFinished())
	{				
		stopPersonChecking();
		kinect().isPoseDetecting = false;
	}
	else
	{
		checkPersonPose();
	}
}

void MainGameScreen::updateGameResult() 
{
	if(showGameResultTimeIsFinished())
	{
		_resultTimer.stop();
		state = MAKE_SCREENSHOOT;			
	}
}

void MainGameScreen::updatePhotoMaking() 
{
	if (cameraCanon().checkIfDownloaded())
	{
		setPlayerOnePoseGuess(cameraCanon().getpathToDownloadedPhoto());			
		checkAnimationFinished();
	}
	else if (cameraCanon().checkIfError())
	{
		setPlayerOnePoseGuess();
		checkAnimationFinished();
	}
}

void MainGameScreen::setPlayerOnePoseGuess(std::string pathToHiRes) 
{
	PlayerData::score++;
	PlayerData::playerData[currentPose - 1].isFocusError = false;
	PlayerData::playerData[currentPose - 1].isSuccess	 = true;
	PlayerData::playerData[currentPose - 1].pathHiRes	 =  pathToHiRes;
	PlayerData::playerData[currentPose - 1].screenshot	 = cameraCanon().getSurface();
	PlayerData::playerData[currentPose - 1].storyCode	 = poseCode;	

}

void MainGameScreen::checkAnimationFinished() 
{
	if (winAnimationFinished == false)
	{					
		state = WIN_ANIMATION_FINISH_WAIT;					
	}
	else state = SHOW_GAME_RESULT;
}


void MainGameScreen::updateAnimationWait() 
{
	if (winAnimationFinished )
	{
		state = SHOW_GAME_RESULT;
	}
}

void MainGameScreen::makeScreenShootUpdtae() 
{
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

void MainGameScreen::checkPersonPose() 
{
	kinect().update();

	if (1)// kinect().getPoseProgress() >= MATCHING_MAX_VALUE)
	{
		kinect().poseComplete();
		stopPersonChecking();
		_onePoseTimer.stop();		
		kinect().isGameRunning = false;
		winAnimationFinished = false;	

		state = PHOTO_MAKING_WAIT;	
		timeline().apply(&alphaFlashAnim, 1.0f,  0.0f, 1.5f , EaseInCubic()).finishFn( bind( &MainGameScreen::animationFinished, this ) ).delay(1.5);			

		cameraCanon().takePhoto();
	}
}

void MainGameScreen::animationFinished() 
{
	winAnimationFinished = true;
	_resultTimer.start();	
}

void 	MainGameScreen::stopPersonChecking() 
{
	_onePoseTimer.stop();
	_resultTimer.start();
	kinect().isGameRunning = false;
	state = SHOW_GAME_RESULT;
}

bool MainGameScreen::allPoseDone() 
{
	return currentPose >= POSE_IN_GAME_TOTAL;
}

void MainGameScreen::gotoResultScreen() 
{
	_game->changeState(ResultScreen::Instance());
}

void MainGameScreen::nextPose() 
{
	currentPose++;
	poseCode = kinect().nextPose();	
}

void MainGameScreen::draw() 
{
	gl::enableAlphaBlending();	

	if (cameraCanon().isConnected == false)
	{
		drawCameraLostImage();
		stopAllTimersIfNeed();
		return;
	}
	
	cameraCanon().draw();
	//drawCameraImage();	

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
	
	#ifdef debug
		//drawDebugMessage();
	#endif	

	if (personisFound() == false)
	{
		Utils::textFieldDraw("Èãðîê ïîêèíóë íàñ. âîçâðàò ÷åðåç  "+to_string((int)(Params::comeBackHomeTime - _missedTimer.getSeconds())), &debugFont26, Vec2f(0.f, 900.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	}

	comeBackBtn->draw();

	gl::disableAlphaBlending();
}

void MainGameScreen::drawCameraImage()
{
	kinect().drawKinectCameraColorSurface();
}

void MainGameScreen::drawFirstMessageBox()
{
	Utils::textFieldDraw("ÎÒÎÉÄÈÒÅ ÏÎÄÀËÜØÅ", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void MainGameScreen::drawPreReadyCounterBox()
{
	gl::disableAlphaBlending();
	Utils::textFieldDraw("ÏÐÈÃÎÒÎÂÜÒÅÑÜ Ê ÏÎÇÅ "+to_string(currentPose)+" : "+to_string(PREGAME_TIME - (int)_preGameTimer.getSeconds()), &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	gl::enableAlphaBlending();
}

void MainGameScreen::drawPoseSilhouette()
{
	kinect().drawLoadedPoses();
	kinect().drawSkeletJoints();
	gl::disableAlphaBlending();
	gl::color(Color::white());
	Utils::textFieldDraw("ÄÎ ÊÎÍÖÀ ÏÎÏÛÒÊÈ ÎÑÒÀËÎÑÜ  "+to_string(ONE_POSE_TIME - (int)_onePoseTimer.getSeconds()), &debugFont26, Vec2f(1300.f, 10.0f), ColorA(1.f, 0.f, 0.f, 1.f));

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
		Utils::textFieldDraw("ÏÎÇÛ ÑÎÂÏÀËÈ ÓÐÀ!!! ( "+to_string(RESULT_TIME - (int)_resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));
		gl::enableAlphaBlending();
		
	}
	else
	{
		gl::draw(failImage, centeredRect);
		gl::disableAlphaBlending();
		Utils::textFieldDraw("ÑÒÀÐÀÉÒÅÑÜ ËÓ×ØÅ ( "+to_string(RESULT_TIME - (int)_resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));	
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
	//gl::translate(kinect().viewShiftX, kinect().viewShiftY);	
	//gl::scale(kinect().headScale, kinect().headScale);
	gl::translate(cameraCanon().getSurfaceTranslate());
	gl::scale(-cameraCanon().scaleFactor, cameraCanon().scaleFactor);
	gl::draw(PlayerData::playerData[currentPose-1].screenshot);
	gl::popMatrices();


	gl::pushMatrices();
		gl::translate(getWindowWidth() - 360.0, getWindowHeight() - 512.0);		
		gl::draw(kinect().getPoseImage());
	gl::popMatrices();


	/*if (poseCode == 1)
	{
		gl::pushMatrices();

		gl::translate(getWindowWidth() - 360.0, getWindowHeight() - 512.0);		
		gl::draw(comics1_godzilla);
		gl::popMatrices();
	}
	else if (poseCode == 2)
	{
		gl::pushMatrices();
		gl::translate(getWindowWidth() - comics2_cat.getWidth(), getWindowHeight() - comics2_cat.getHeight());
		gl::draw(comics2_cat);
		gl::popMatrices();

	}
	else if (poseCode == 3)
	{
		gl::pushMatrices();
		gl::translate(getWindowWidth() - comics3_uni.getWidth(), getWindowHeight() - comics3_uni.getHeight());
		gl::draw(comics3_uni);
		gl::popMatrices();
	}*/
}

void MainGameScreen::drawPhotoFlash()
{
	gl::color(ColorA(1, 1, 1, alphaFlashAnim));
	gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
	gl::color(Color::white());
}

void MainGameScreen::drawDebugMessage()
{
	gl::color(Color::white());
	string outString1	 = "state : " + stateMemoMap[state];
	Utils::textFieldDraw(outString1, &debugFont26, Vec2f(10.f, 10.0f), ColorA(1.f, 1.f, 1.f, 1.f));

	string outString2	 = "Ëþäåé â êàäðå : " + to_string(peopleCount) +
						   " | Ðàññòîÿíèå äî áëèæàéøåãî èãðîêà : " +  to_string(distanceToPlayer)+
						   " | Òåêóùàÿ ïîçà : " +  to_string(currentPose)+"/"+  to_string(ALL_POSES_COUNT);


	Utils::textFieldDraw(outString2, &debugFont26, Vec2f(10.f, 40.0f), ColorA(1.f, 1.f, 1.f, 1.f));

	string outString3	 = "ÒÀÉÌÅÐÛ ::::::: \n| Òàéìåð ÷òîáû îòîéòè : " + to_string(_preWaitingTimer.getSeconds()) +
		" \n| Ïîäãîòîâèòåëüíûé òàéìåð : "+ to_string((int)_preGameTimer.getSeconds()) +
		" \n| Òàéìåð äëÿ îäíîé ïîçû "+  to_string((int)_onePoseTimer.getSeconds()) +
		" \n| Òàéìåð ïîêàçà ðåçóëüòàòà "+  to_string((int)_resultTimer.getSeconds());

	Utils::textFieldDraw(outString3, &debugFont26, Vec2f(10.f, 100.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void MainGameScreen::drawCameraLostImage()
{
	Utils::textFieldDraw("ÊÀÌÅÐÀ ÂÛÊËÞ×ÅÍÀ", &debugFont26, Vec2f(10.f, 10.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void MainGameScreen::stopAllTimersIfNeed()
{
	if(!_preWaitingTimer.isStopped()) _preWaitingTimer.stop();
	if(!_preGameTimer.isStopped()) _preGameTimer.stop();
	if(!_onePoseTimer.isStopped()) _onePoseTimer.stop();
	if(!_resultTimer.isStopped()) _resultTimer.stop();
	if(!_missedTimer.isStopped()) _missedTimer.stop();
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

void MainGameScreen::gotoFirstScreen() 
{
	//isChangingStateNow = true;
	//timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &MainGameScreen::animationFinished, this ) );	
	_game->changeState(IntroScreen::Instance());
}