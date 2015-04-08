#pragma once
#pragma warning(push)
#pragma warning(disable: 4244)

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "Saver.h"
#include "Playerdata.h"
#include "KinectAdapter.h"
#include "HintScreen.h"
#include "GameControlScreen.h"
#include "ComicsScreen.h"
#include "CameraAdapter.h"

namespace gameStates
{
	enum states
	{
		NONE, 
		STEP_BACK_MESSAGE,
		HINT_MESSAGE,
		PRE_GAME_INTRO,
		HANDS_UP_AWAITING,
		MAIN_GAME, 
		COUNTERS_ANIMATE,
		SHOW_GAME_RESULT,
		PHOTO_MAKING_WAIT,
		WIN_ANIMATION_FINISH_WAIT,
		MAKE_SCREENSHOOT,
		COUNTER_STATE
	};
}

using namespace ci;
using namespace ci::gl;
using namespace std;
using namespace gameStates;
using namespace boost::signals2;

//#define debug

class Game
{

public:	
		
	static Game& getInstance() { static Game game; return game; };

#ifdef debug
	static const int	STEP_BACK_TIME  = 4;
#else
	static const int	STEP_BACK_TIME  = 40;
#endif

	static const int    HANDS_UP_AWAITINTING_TIME = 40;
	static const int    STEP_BACK_ENSURE_TIME = 2;

	static const int	PREGAME_TIME = 2;
	
	static const int	COUNTDOWN_TIME		= 3;
	static const int	HINT_TIME			= 3;
	static const int	RESULT_TIME			= 4;
	static const int	COUNTERS_ANIM_TIME	= 2;

	static const int	LEVEL_PASS_INC		= 4;
	static const int	LEVEL_PASS_DEC		= 6;

	int CURRENT_POSE_TIME;
	int CURRENT_MATCHING_PERCENT;
	int state;
	int allHumanPointsInScreenRectStable;
	float scalePlus;

	bool isGameRunning, isPoseDetecting, winAnimationFinished, _onePoseTimerPause;
	Timer  _preGameTimer, _onePoseTimer, _resultTimer, _stepBackTimer, _hintTimer;
	Timer _handsUpAwaitingTimer, _stepBackEnsureTimer, _countDownTimer, _countersAnimTimer;

	Pose* foundPose, currentPose;
	vector<Pose*> poses;
	size_t poseNum;
	int poseCode, level, levelCompletion;
	float mathPercent, etalonHeight;	
	float scaleAccordingUserHeight;

	signal<void(void)> gotoResultScreenEvent;
	signal<void(void)> gotoFirstScreenEvent;	
	signal<void(void)> photoFlashEvent;
	signal<void(void)> cameraStartUpdateEvent;
	signal<void(void)> cameraStopUpdateEvent;

	ci::signals::connection quickAnimationFinishedSignal;

	bool testPercent100, isHandsUp;	
	string debugString;

	vector<Vec3f> skelet;
	Rectf districtKinectRectf;

	void setup();
	void initnew();
	void update();

	void updateStepBackMessage();
	void initHandsUpStateParams();
	bool allStpeBackConditionsGood();
	void updateHandsAwaiting();

	bool lostForHandsUpDetectionMode();
	void initStepBackStateParams();
	void calculateCurrentUserScale();
	void updatePreGameIntro();

	void updateCounterState();
	void updateCountersAnimState();
	void updateMainGame();

	bool isLevelFailed();
	void updateHintMessage();

	void updateMakeScreenShoot();
	void updateGameResult();

	void updatePhotoMaking();
	void checkAnimationFinished();
	void updateAnimationWait();
	void gotoLevelCompleteScreen();
	void flashAnimationFinished();

	///////////////////////////////////////////////////////////////////
	//
	//			POSE DETECTION
	//
	///////////////////////////////////////////////////////////////////

	void initPoseData();
	void drawJoints();
	int generatePoseCode();

	void checkPersonPose();
	void updateGame();
	float getPoseProgress();
	void drawCurrentSilhouette();
	Texture getPoseImageById(int id);
	bool getHasMaskStatus(int id);
	string getPoseIndex();
	Surface getCurrentScreenShot();
	void matchTemplate();
	void stopPersonChecking();
	void computeMistakeWay1();
	double getMatchPercent();
	bool isPointsSizeEqual(const Pose& pose1, const Pose& pose2);
	bool isAnchorPointsMatch(const Pose& pose1, const Pose& pose2);
	double calculateDistanceBetweenPoints(const Vec3f& vec1, const Vec3f& vec2);
	void setPlayerOnePoseGuess(const std::string& pathToHiRes = "", bool _focusError = false);
	void drawCurrentPlayerJoints();

	///////////////////////////////////////////////////////////////////
	//
	//			TIMERS
	//
	///////////////////////////////////////////////////////////////////

	void stopAllTimersIfNeed();
	bool isTimerFinished(Timer timer, int endTime);

	///////////////////////////////////////////////////////////////////
	//
	//			POSE SAVING
	//
	///////////////////////////////////////////////////////////////////

	void saveAsTemplate(int num);
};

inline Game&	recognitionGame() { return Game::getInstance(); };

#pragma warning(pop)