#pragma once
#pragma warning(push)
#pragma warning(disable: 4244)

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "Pose.h"
#include "AssetsManager.h"
#include "KinectAdapter.h"

class Pose;

class GameControlScreen
{
public:

	static GameControlScreen& getInstance() { static GameControlScreen game; return game; };

	ci::Anim<float> silhouetteAlpha, matchingPopupAlpha;
	ci::Anim<ci::Vec2f> timerVec, plashkaVec;

	ci::gl::Texture sidePlashka, timer_bubble, popka;
	ci::gl::Texture	silhouette, percentTexure, matchingPopup, palka;

	ci::Font secondsFont, debugFontText, percentFont;

	float quickAnimPosePercent, matchingProgress, palkaPosition;
	int percentMatching, quickAnimTime, state, showingSeconds, startingTime;

	ci::Vec2f animationPosition;
	Pose *currentPose;

	float poseScale;
	ci::Vec2f poseShiftVec;

	static const int circlesNum = 3;

	enum{
		RUNNING,
		FINISHING,
		HIDING,
		STATIC_SHOWING,
		QUICK_ANIMATION
	};

	struct col
	{
		float r, g, b, a;
	}purpurColor, greyColor;

	void setup();
	void init();
	void draw();
	void drawYellowCountur();
	void drawCountDownSeconds();
	void drawLeftPercentPanel();
	void drawMatchingPopup();
	void drawMatchingBubbles();
	void setTimeForAnimation(int seconds);
	void show();
	void show1();
	void show2();
	void initStartAnimation();
	void initStartAnimation1();
	void quickAnimation(int timeToQuickAnimate);
	void quickAnimationSetProgress(float percent);
	void setTime(int seconds);
	void setShowingTime(int seconds);
	void setQuickAnimTime(int time);
	void setQuickAnimPosePercent(float percent);
	void run();
	void start();
	void hide();
	void setDetentionPercent(float percent);
	void showSilhouette();
	void setCurrentPose(Pose *pose);
	void showMatching(float  progressInt);
	void setPoseScale(float poseScale);
	void setPoseShift(const ci::Vec2f& poseShiftVec);
};

inline GameControlScreen&	gameControls() { return GameControlScreen::getInstance(); };

#pragma warning(pop)