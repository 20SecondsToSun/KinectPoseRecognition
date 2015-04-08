#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "AssetsManager.h"
#include "Utils.h"
#include "Params.h"

class HintScreen
{
public:
	static HintScreen& getInstance() { static HintScreen game; return game; };

	ci::Anim<float>	 alphaTint, bubbleAlpha, arrowAlpha, bgAlpha, alphaBg;
	ci::Anim<float>	readyTextAlpha, startCounterAlpha, lastAlpha, hintAlpha;
	ci::Anim<float> rotationComicsAnimate;

	ci::Anim<ci::Vec2f>	 scaleComicsAnimate, positionComicsAnimate;
	ci::Anim<ci::Vec2f>	 catVec, arrowVec, bubbleScale;
	ci::Anim<ci::Vec2f>	 manHandsUpTexAnimateVec, handsUpTextTexAnimateVec;

	ci::Vec2f arrowScale;

	ci::gl::Texture hint3, bg, catImage, bubbleImage, arrowImage, screenshot;
	ci::gl::Texture readyTex, countDownTexture, levelNumTexture;
	ci::gl::Texture failImage, bg_blue, manHandsUpTex, handsUpTextTex;
	ci::gl::Texture  *counterPtr, pozeNum;

	std::vector<ci::gl::Texture> counterTexVec;

	int state, gameLevel, sign, poseNum;
	
	ci::Font countDownFont, levelNumFont;

	enum
	{
		SHOW_STEP_BACK,
		SHOW_TIMER,
		SHOW__COUNTDOWN,
		SHOW_NUMS,
		SHOW_READY,
		HANDS_AWAITING,
		FADE_NUMS,
		PREVIEW_ONLY,
		START_HINT,
		SHOW_HINT
	};

	struct col
	{
		float r, g, b, a;
	}
	blue;

	class Preview
	{
		ci::gl::Texture background, photo;
		ci::Vec2f position;
		float _scale;
		bool photoEnabled;

	public:
		Preview();
		void setBackground(const ci::gl::Texture& tex);
		void setPhoto(const ci::gl::Texture& photo);
		void clearTexture();
		void setPosition(const ci::Vec2f& vec);
		void draw();
	};

	std::vector<Preview> previewVec;

	void setup();
	void draw();
	void drawStepBackGraphics();
	void drawAreYoureReadyGraphics();
	void drawHandsUpGraphics();
	void drawCountDownTimer();
	void drawTitle();
	void drawPreview(bool isShadow = true);
	void init();
	void startReadySate(const ci::gl::Texture& texture);
	void startReadySate();
	void startHandsAwaiting();
	void startCountDown();
	void updateCountDown(int time);
	void fadeCounter();
	void startHint();
	void fadeHint();
};

inline HintScreen&	hintScreen() { return HintScreen::getInstance(); };