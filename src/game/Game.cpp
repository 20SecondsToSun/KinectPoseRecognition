#include "Game.h"

void Game::setup()
{
	
	scalePlus = 0.0f;

	poses = saver().loadPoseBase();
	poseNum = 0;
	testPercent100 = false;

	kinect().gestureEvent.connect([&]()
	{
		console() << "hands up gesture --------------------------->  " << endl;
		isHandsUp = true;
	});
}

void Game::initnew()
{
	kinect().startTracking();
	PlayerData::initData();

	level = 1;
	poseNum = 0;
	scaleAccordingUserHeight = 1.0f;
	initPoseData();

	_stepBackTimer.start();
	_stepBackEnsureTimer.start();
	state = STEP_BACK_MESSAGE;// PRE_GAME_INTRO;// STEP_BACK_MESSAGE;
	districtKinectRectf = Rectf(500.0f, 0.0f, 1200.0f, 1080.0f);
}

void Game::update()
{
	kinect().updateSkeletonData(districtKinectRectf);
	skelet = kinect().getCurrentSkelet();

	switch (state)
	{
	case STEP_BACK_MESSAGE:
		districtKinectRectf = Rectf(500.0f, 0.0f, 1200.0f, 1080.0f);
		updateStepBackMessage();
		break;

	case HINT_MESSAGE:
		updateHintMessage();
		break;

	case PRE_GAME_INTRO:
		updatePreGameIntro();
		break;

	case HANDS_UP_AWAITING:
		districtKinectRectf = Rectf(500.0f, 0.0f, 1200.0f, 1080.0f);
		updateHandsAwaiting();
		break;

	case COUNTER_STATE:
		updateCounterState();
		break;

	case COUNTERS_ANIMATE:
		updateCountersAnimState();
		break;

	case MAIN_GAME:
		districtKinectRectf = Rectf(250.0f, 0.0f, 1000.0f, 1080.0f);
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
		updateMakeScreenShoot();
		break;
	}
}

void Game::updateStepBackMessage()
{
	if (allStpeBackConditionsGood())
	{
		initHandsUpStateParams();
	}
	else if (isTimerFinished(_stepBackTimer, STEP_BACK_TIME))
	{
		gotoFirstScreenEvent();
	}
}

void Game::initHandsUpStateParams()
{
	hintScreen().startHandsAwaiting(); // show hint
	kinect().enbleGestures(); // start recognition gestures
	isHandsUp = false;
	allHumanPointsInScreenRectStable = 0;

	_handsUpAwaitingTimer.start();
	state = HANDS_UP_AWAITING;
}

bool Game::allStpeBackConditionsGood()
{
#ifndef debug
	return isTimerFinished(_stepBackEnsureTimer, STEP_BACK_ENSURE_TIME)
		&& kinect().isDistanceOk()
		&& kinect().allHumanPointsInScreenRect();
#else
	return isTimerFinished(_stepBackEnsureTimer, STEP_BACK_ENSURE_TIME);
#endif
	//&& kinect().isHeapInRect(districtKinectRectf);
}

void Game::updateHandsAwaiting()
{
#ifndef debug
	if (!kinect().allHumanPointsInScreenRect())
		allHumanPointsInScreenRectStable++;

	if (lostForHandsUpDetectionMode())
	{
		initStepBackStateParams();
	}
	else if (isTimerFinished(_handsUpAwaitingTimer, HANDS_UP_AWAITINTING_TIME))
	{
		gotoFirstScreenEvent();
		kinect().disableGestures();
	}
	else if (isHandsUp)
#endif
	{
		_handsUpAwaitingTimer.stop();
		kinect().disableGestures();

		calculateCurrentUserScale();

		hintScreen().gameLevel = level;
		hintScreen().startReadySate();

		poses[poseCode]->calculateShifts(scaleAccordingUserHeight);
		poses[poseCode]->scalePoints(scaleAccordingUserHeight);

		gameControls().setPoseScale(scaleAccordingUserHeight);
		gameControls().setPoseShift(poses[poseCode]->getPoseShift());

		_preGameTimer.start();
		state = PRE_GAME_INTRO;
	}
}

bool Game::lostForHandsUpDetectionMode()
{
	return !kinect().isDistanceOk() || allHumanPointsInScreenRectStable > 10;
}

void Game::initStepBackStateParams()
{
	_stepBackTimer.start();
	_stepBackEnsureTimer.start();
	_handsUpAwaitingTimer.stop();
	kinect().disableGestures();
	hintScreen().init();
	state = STEP_BACK_MESSAGE;
}

void Game::calculateCurrentUserScale()
{
	scaleAccordingUserHeight = 1.0f;

	float etalonHeight = kinect().getEtalonHeightInPixelsAccordingDepth();
	float userHeight = kinect().userHeightInPixels();

	if (etalonHeight)
		scaleAccordingUserHeight = Utils::clamp(userHeight / etalonHeight, 1.0f, 0.6f);
	else
		scaleAccordingUserHeight = 1.0f;

	if(scaleAccordingUserHeight>1)
		scaleAccordingUserHeight =1;

#ifdef debug
	//scaleAccordingUserHeight = .6f + scalePlus;
	//scalePlus += 0.1f;
	//scalePlus >= 0.5f ? scalePlus = 0 : scalePlus = scalePlus;
#endif
}

void Game::updatePreGameIntro()
{
	if (isTimerFinished(_preGameTimer, PREGAME_TIME))
	{
		_countDownTimer.start();

		hintScreen().startCountDown();
		state = COUNTER_STATE;
	}
}

void Game::updateCounterState()
{
	if (isTimerFinished(_countDownTimer, COUNTDOWN_TIME))
	{
		gameControls().setTime(CURRENT_POSE_TIME);
		gameControls().setQuickAnimTime(CURRENT_POSE_TIME + 40);
		gameControls().setQuickAnimPosePercent(.41f);
		gameControls().showSilhouette();

		if (level == 1)
		{
			cameraStartUpdateEvent();
			_hintTimer.start();
			hintScreen().startHint();
			gameControls().show1();

			state = HINT_MESSAGE;
		}
		else
		{
			cameraStartUpdateEvent();
			gameControls().show();
			_countersAnimTimer.start();
			hintScreen().fadeCounter();
			state = COUNTERS_ANIMATE;
		}
	}
	else
	{
		hintScreen().updateCountDown(COUNTDOWN_TIME - (int)_countDownTimer.getSeconds());
	}
}

void Game::updateCountersAnimState()
{
	if (isTimerFinished(_countersAnimTimer, COUNTERS_ANIM_TIME))
	{
		_onePoseTimer.start();

		isPoseDetecting = false;
		isGameRunning = true;
		_onePoseTimerPause = false;
		mathPercent = 0.0f;
		state = MAIN_GAME;
	}
	else
	{
		gameControls().quickAnimationSetProgress(_countersAnimTimer.getSeconds());
	}
}

void Game::updateMainGame()
{
	if (isLevelFailed())
	{
		gameControls().setShowingTime(0);
		stopPersonChecking();
		isPoseDetecting = false;
		gotoLevelCompleteScreen();
	}
	else if (_onePoseTimerPause)
	{
		gameControls().setShowingTime(CURRENT_POSE_TIME);
		gameControls().setDetentionPercent(mathPercent);
		checkPersonPose();
	}
	else
	{
		gameControls().setShowingTime(CURRENT_POSE_TIME - (int)_onePoseTimer.getSeconds());
		gameControls().setDetentionPercent(mathPercent);
		checkPersonPose();
	}
}

bool Game::isLevelFailed()
{
	return isTimerFinished(_onePoseTimer, CURRENT_POSE_TIME) && !_onePoseTimerPause;
}

void Game::updateHintMessage()
{
	if (isTimerFinished(_hintTimer, HINT_TIME))
	{
		_countersAnimTimer.start();
		hintScreen().fadeHint();
		gameControls().show2();
		state = COUNTERS_ANIMATE;
	}
}

void Game::updateMakeScreenShoot()
{
	if (level++ >= POSE_IN_GAME_TOTAL)
	{
		state = NONE;
		gotoResultScreenEvent();
	}
	else
	{
		hintScreen().gameLevel = level;
		hintScreen().startReadySate(comicsScreen().getCurrentComics());
		initPoseData();

		state = PRE_GAME_INTRO;
		_preGameTimer.start();
	}
}

void Game::updateGameResult()
{
	if (isTimerFinished(_resultTimer, RESULT_TIME))
	{
		_resultTimer.stop();
		state = MAKE_SCREENSHOOT;
	}
}

void Game::updatePhotoMaking()
{
#ifndef nocamera
	if (cameraCanon().checkIfDownloaded())
	{
		setPlayerOnePoseGuess(cameraCanon().getpathToDownloadedPhoto());
		checkAnimationFinished();
	}
	else if (cameraCanon().checkIfError())
	{
		setPlayerOnePoseGuess("", true);
		checkAnimationFinished();
	}
#else
	setPlayerOnePoseGuess("", true);
	checkAnimationFinished();
#endif
}

void Game::checkAnimationFinished()
{
	if (winAnimationFinished)
		gotoLevelCompleteScreen();
	else
		state = WIN_ANIMATION_FINISH_WAIT;
}

void Game::updateAnimationWait()
{
	if (winAnimationFinished)
		gotoLevelCompleteScreen();
}

void Game::gotoLevelCompleteScreen()
{
	gameControls().hide();
	comicsScreen().setGuess(isPoseDetecting);

	if (isPoseDetecting)
	{
		comicsScreen().setCameraImage(PlayerData::playerData[level - 1].screenshot);
		comicsScreen().setComicsImage(poses[poseCode]->getComicsImage());
		comicsScreen().setMiddlePoint(poses[poseCode]->getMidlePoint());
		comicsScreen().setPoseScale(scaleAccordingUserHeight);
		comicsScreen().setPoseShift(poses[poseCode]->getPoseShift());

		try
		{
			comicsScreen().createResultComics(level - 1);
			state = SHOW_GAME_RESULT;
		}
		catch (...)
		{
			state = NONE;
			gotoResultScreenEvent();
		}
	}
	else
	{		
		if (level == POSE_IN_GAME_TOTAL && PlayerData::getPlayerScore() == 0)
		{
			state = NONE;
			gotoResultScreenEvent();
		}
		else
		{
			state = SHOW_GAME_RESULT;
			comicsScreen().createResultComicsFail(level - 1);			
		}
					
	}
}

void Game::flashAnimationFinished()
{
	winAnimationFinished = true;
	_resultTimer.start();
}

///////////////////////////////////////////////////////////////////
//
//			POSE DETECTION
//
///////////////////////////////////////////////////////////////////

void Game::initPoseData()
{
	poseCode = generatePoseCode();

	CURRENT_POSE_TIME = poses[poseCode]->getPoseTime();
	CURRENT_MATCHING_PERCENT = poses[poseCode]->getPercent();
	poses[poseCode]->calculateShifts(scaleAccordingUserHeight);
	poses[poseCode]->scalePoints(scaleAccordingUserHeight);

	gameControls().setCurrentPose(poses[poseCode]);
	gameControls().setPoseScale(scaleAccordingUserHeight);
	gameControls().setPoseShift(poses[poseCode]->getPoseShift());
}

int Game::generatePoseCode()
{
	int code = poseNum++;
	if (poseNum > poses.size() - 1)
		poseNum = 0;	

	return code;//level - 1; //TODO
}

void Game::checkPersonPose()
{
	updateGame();

	if (getPoseProgress() >= CURRENT_MATCHING_PERCENT + 10)
	{
		gameControls().showMatching(1.0f);
		isPoseDetecting = true;
		isGameRunning = false;
		winAnimationFinished = false;

		levelCompletion = 0;

		stopPersonChecking();
		_onePoseTimer.stop();

		state = PHOTO_MAKING_WAIT;
		photoFlashEvent();
		cameraCanon().takePhoto();
	}
}

void Game::updateGame()
{
	if (isGameRunning)
	{
		matchTemplate();


		if (testPercent100)
			mathPercent = 1;

		if (mathPercent > Params::percentForMatching)
		{
			levelCompletion += LEVEL_PASS_INC;

			if (_onePoseTimerPause == false)
			{
				CURRENT_POSE_TIME -= (int)_onePoseTimer.getSeconds();

				if (_onePoseTimer.isStopped() == false)
					_onePoseTimer.stop();

				_onePoseTimerPause = true;
			}
		}
		else
		{
			if (_onePoseTimer.isStopped() == true)
				_onePoseTimer.start();

			_onePoseTimerPause = false;

			if (levelCompletion - LEVEL_PASS_DEC >= 0)
				levelCompletion -= LEVEL_PASS_DEC;
			else
				levelCompletion = 0;
		}

		gameControls().showMatching((float)levelCompletion / CURRENT_MATCHING_PERCENT);
	}
}

float Game::getPoseProgress()
{
	return floor(levelCompletion);
}

void Game::drawCurrentSilhouette()
{
	return poses[poseCode]->draw();
}

Texture Game::getPoseImageById(int id)
{
	return poses[id]->getComicsImage();
}

bool Game::getHasMaskStatus(int id)
{
	return poses[id]->isPoseWithMask();
}

string Game::getPoseIndex()
{
	return to_string(poses.size() + 1);
}

Surface Game::getCurrentScreenShot()
{
	return PlayerData::playerData[level].screenshot;
}

void Game::matchTemplate()
{
	foundPose = NULL;

	SkeletJoints skelet = kinect().getCurrentSkelet();

	if (skelet.size() == 0)
	{
		mathPercent = 0.0f;
		return;
	}

	currentPose.setPoints(skelet);
	currentPose.createNormalizePoints0();
	currentPose.initColors();

	if (isPointsSizeEqual(*poses[poseCode], currentPose))
	{
		computeMistakeWay1();
	}
	else
	{
		mathPercent = 0.0f;
	}
}

void Game::stopPersonChecking()
{
	_onePoseTimer.stop();
	_resultTimer.start();
	isGameRunning = false;
}

void Game::computeMistakeWay1()
{
	mathPercent = 0.0f;

	for (size_t j = 0, len = currentPose.getNormalizePoints().size(); j < len; ++j)
	{
		double mistake = calculateDistanceBetweenPoints(poses[poseCode]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
		double onePartPercent = 0;
		double onePart = Params::weightJoints[j];

		if (mistake >= Params::maxErrorBetweenJoints)
		{
			onePartPercent = 0;
		}
		else if (mistake < Params::minErrorBetweenJoints)
		{
			currentPose.setPointColor(j, ColorA(1.0f, 1.0f, 1.0f, 0.0f));
			onePartPercent = onePart;
		}
		else
		{
			float norma = (Params::maxErrorBetweenJoints - mistake) / (Params::maxErrorBetweenJoints - Params::minErrorBetweenJoints);
			onePartPercent = onePart * (1 - norma);
			currentPose.setPointColor(j, ColorA(0.0f, 0.0f, 1.0f, abs(onePartPercent * 10)));
		}

		mathPercent += onePartPercent;
	}
}

double Game::getMatchPercent()
{
	return mathPercent * 100;
}

bool Game::isPointsSizeEqual(const Pose& pose1, const Pose& pose2)
{
	return pose1.getNormalizePoints().size() == pose2.getNormalizePoints().size();
}

bool Game::isAnchorPointsMatch(const Pose& pose1, const Pose& pose2)
{
	Vec2f anchor1 = pose1.getAnchor();
	Vec2f anchor2 = pose2.getAnchor();

	return (abs(anchor1.x - anchor2.x) < Params::boxMaxErrorX && abs(anchor1.y - anchor2.y) < Params::boxMaxErrorY);
}

double Game::calculateDistanceBetweenPoints(const Vec3f& vec1, const Vec3f& vec2)
{
	return abs((Vec2f(vec1.x, vec1.y) - Vec2f(vec2.x, vec2.y)).length());
}

void Game::setPlayerOnePoseGuess(const std::string& pathToHiRes , bool _focusError)
{
	PlayerData::score++;
	PlayerData::playerData[level - 1].isFocusError = _focusError;
	PlayerData::playerData[level - 1].isSuccess = true;
	PlayerData::playerData[level - 1].pathHiRes = pathToHiRes;
#ifndef nocamera
	PlayerData::playerData[level - 1].screenshot = cameraCanon().getSurface();
#else
	PlayerData::playerData[level - 1].screenshot = copyWindowSurface();
#endif
	PlayerData::playerData[level - 1].storyCode = poseCode;

	if (_focusError == false) PlayerData::photosWithoutError++;
}

void Game::drawJoints()
{
	// compare pose points
	gl::pushMatrices();
	gl::translate(kinect().viewShiftX, kinect().viewShiftY);
	gl::scale(kinect().headScale, kinect().headScale);
	poses[poseCode]->drawPoints();
	gl::popMatrices();
}

void Game::drawCurrentPlayerJoints()
{
	gl::drawStrokedRect(districtKinectRectf);
	if (skelet.size() == 0)
		return;

	gl::pushMatrices();
	gl::translate(kinect().viewShiftX, kinect().viewShiftY);
	gl::scale(kinect().headScale, kinect().headScale);
	currentPose.setPoints(skelet);
	currentPose.createNormalizePoints0();
	currentPose.initColors();
	currentPose.drawPoints();
	gl::popMatrices();
	Utils::textFieldDraw(debugString, fonts().getFont("Helvetica Neue", 46), Vec2f(40.0f, 40.0f), Color(1.0f, 0.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////
//
//			TIMERS
//
///////////////////////////////////////////////////////////////////

void Game::stopAllTimersIfNeed()
{
	if (!_stepBackTimer.isStopped()) _stepBackTimer.stop();
	if (!_hintTimer.isStopped()) _hintTimer.stop();
	if (!_preGameTimer.isStopped()) _preGameTimer.stop();
	if (!_countDownTimer.isStopped()) _countDownTimer.stop();
	if (!_countersAnimTimer.isStopped()) _countersAnimTimer.stop();
	if (!_onePoseTimer.isStopped()) _onePoseTimer.stop();
	if (!_resultTimer.isStopped()) _resultTimer.stop();
	if (!_handsUpAwaitingTimer.isStopped()) _handsUpAwaitingTimer.stop();
	if (!_stepBackEnsureTimer.isStopped()) _stepBackEnsureTimer.stop();
}

bool Game::isTimerFinished(Timer timer, int endTime)
{
	if (timer.isStopped() || (!timer.isStopped() && timer.getSeconds() > endTime))
	{
		timer.stop();
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////
//
//			POSE SAVING
//
///////////////////////////////////////////////////////////////////

void Game::saveAsTemplate(int num)
{
	string poseName = "Cat" + to_string(num); // + 1;	

	if (kinect().getCurrentSkelet().size() && kinect().getSilhouette())
	{
		Pose* pose = new Pose();
		pose->setName(poseName);
		pose->setPoints(kinect().getCurrentSkelet());
		pose->setKinectTilt(kinect().getTilt());
		pose->setImage(kinect().getSilhouette());
		pose->setTime(30);
		pose->setPercent(80);
		pose->setComicsName("default");
		saver().savePoseIntoBase(pose);
	}
}