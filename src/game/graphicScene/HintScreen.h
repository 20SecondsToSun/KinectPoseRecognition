#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "AssetsManager.h"

using namespace ci;
using namespace gl;
using namespace std;

class HintScreen
{
public:	

	static HintScreen& getInstance() { static HintScreen game; return game; };

	Anim<float>	 alphaTint, bubbleAlpha, arrowAlpha, bgAlpha;
	Anim<float>	readyTextAlpha, startCounterAlpha, lastAlpha, hintAlpha;
	Anim<float> rotationComicsAnimate;

	Anim<Vec2f>	 scaleComicsAnimate, positionComicsAnimate;	
	Anim<Vec2f>	 catVec, arrowVec, bubbleScale;
	Anim<Vec2f>	 manHandsUpTexAnimateVec, handsUpTextTexAnimateVec;

	Vec2f arrowScale;

	Texture hint3, bg, catImage, bubbleImage, arrowImage, screenshot;
	Texture readyTex, countDownTexture, levelNumTexture;
	Texture failImage, bg_blue, manHandsUpTex, handsUpTextTex;
	Texture counter1Tex, counter2Tex, counter3Tex, *counterPtr;

	int state, gameLevel, sign;

	Font countDownFont, levelNumFont;

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
		float r,g,b,a;
	}blue;
	/*struct previewParams
	{
	Vec2f position;
	Vec2f scale;
	float rotation;
	Texture screenshot;
	};*/

	//previewParams previewVec[3];

	void setup()
	{
		hint3       = *AssetManager::getInstance()->getTexture("images/diz/hint3.png");
		bg          = *AssetManager::getInstance()->getTexture("images/diz/bg.jpg");
		catImage    = *AssetManager::getInstance()->getTexture("images/diz/cat3.png");
		bubbleImage = *AssetManager::getInstance()->getTexture("images/diz/bubble2.png");
		arrowImage  = *AssetManager::getInstance()->getTexture("images/diz/arrow2.png");
		readyTex    = *AssetManager::getInstance()->getTexture("images/diz/readyText.png");
		//failImage   = *AssetManager::getInstance()->getTexture("images/fail.jpg");
		bg_blue		= *AssetManager::getInstance()->getTexture("images/diz/bg.jpg");

		countDownFont= Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 650);
		levelNumFont = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 63);

		manHandsUpTex    = *AssetManager::getInstance()->getTexture("images/diz/handsUpMan.png");
		handsUpTextTex	 = *AssetManager::getInstance()->getTexture("images/diz/handsUpText.png");

		counter1Tex  = *AssetManager::getInstance()->getTexture("images/diz/__1.png");
		counter2Tex  = *AssetManager::getInstance()->getTexture("images/diz/__2.png");
		counter3Tex  = *AssetManager::getInstance()->getTexture("images/diz/__3.png");

		blue.r = 235.0f/255.0f;
		blue.g = 237.0f/255.0f;
		blue.b = 238.0f/255.0f;

		gameLevel = 1;

		//previewParams param;

		//param.position = Vec2f(36.0f, 32.0f);
		//param.scale = Vec2f(0.071f, 0.071f);
		//param.rotation = 0;

		//previewVec[0] =param;

		//param.position = Vec2f(35.0f, 112.0f);
		//param.scale = Vec2f(0.071f, 0.071f);
		//param.rotation = 0;
		//previewVec[1] =param;

		//param.position = Vec2f(35.0f, 306.0f);
		//param.scale = Vec2f(0.0687f, 0.067f);
		//param.rotation = 0;
		//previewVec[2] = param;

		sign = 1;
	}

	void draw()
	{
		if (state == SHOW_STEP_BACK)
		{
			drawStepBackGraphics();
		}
		else if (state == SHOW_READY)
		{
			drawAreYoureReadyGraphics();
		}
		else if (state == HANDS_AWAITING)
		{
			drawHandsUpGraphics();
		}
		else if (state == SHOW_NUMS)
		{
			drawCountDownTimer();
		}
		else if (state == FADE_NUMS)
		{
			//gl::color(ColorA(1.0f, 1.0f, 1.0f, 0));
			//gl::draw(bg_blue);
			//drawTitle();

			gl::color(Color::white());
			drawPreview(false);
		}
		else if (state == PREVIEW_ONLY)
		{
			gl::color(Color::white());
			drawPreview(false);
		}
		else if (state == START_HINT)
		{
			gl::color(Color::white());
			drawPreview(false);

			gl::color(ColorA(1.0f, 1.0f, 1.0f, hintAlpha));
			gl::draw(hint3, Vec2f(1267.0f, 53.0f));
		}
	}

	void drawStepBackGraphics()
	{
		gl::draw(bg);
		gl::color(ColorA(1.0f, 1.0f, 1.0f, bubbleAlpha));
		gl::pushMatrices();
		gl::translate(799.0f, 117.0f);
		gl::scale(bubbleScale);
		gl::draw(bubbleImage);
		gl::popMatrices();
		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(catVec);
		gl::draw(catImage);
		gl::popMatrices();

		gl::color(ColorA(1.0f, 1.0f, 1.0f, arrowAlpha));
		gl::pushMatrices();
		gl::translate(arrowVec);
		gl::translate(-arrowImage.getWidth()*0.5f*arrowScale.x, 0.0f);
		gl::scale(arrowScale);
		gl::draw(arrowImage);
		gl::popMatrices();
		gl::color(Color::white());

		float scaleFactor = 0.035f;

		if (arrowScale.x > 1.15f &&  sign == 1)
		{					
			sign = -1;
		}
		else if (arrowScale.x < 0.6f && sign == -1)
		{				
			sign = 1;
		}

		if (sign == 1)
		{
			arrowScale += Vec2f(scaleFactor, scaleFactor);
		}
		else if (sign == -1)
		{
			arrowScale -= Vec2f(scaleFactor, scaleFactor);
		}
	}

	void drawAreYoureReadyGraphics()
	{
		gl::color(ColorA(blue.r, blue.g, blue.b, 1.0f));
		gl::draw(bg_blue);

		gl::color(ColorA(blue.r, blue.g, blue.b, bgAlpha));
		gl::draw(readyTex, Vec2f(472.0f, 416.0f));		

		//drawTitle();
		//drawPreview();
		gl::color(Color::white());

		if (gameLevel > 1)
		{
			gl::pushMatrices();
			gl::translate(positionComicsAnimate);
			gl::scale(scaleComicsAnimate);
			gl::rotate(rotationComicsAnimate);
			gl::draw(screenshot);
			gl::popMatrices();
		}
	}

	void drawHandsUpGraphics()
	{
		gl::draw(bg);
		gl::color(ColorA(1.0f, 1.0f, 1.0f, hintAlpha));
		gl::draw(manHandsUpTex, manHandsUpTexAnimateVec);
		gl::draw(handsUpTextTex, handsUpTextTexAnimateVec);
		gl::color(Color::white());
	}

	void drawCountDownTimer()
	{
		gl::color(ColorA(blue.r, blue.g, blue.b, 1.0f));
		gl::draw(bg_blue);

		gl::color(ColorA(1.0f, 1.0f, 1.0f, readyTextAlpha));
		gl::draw(readyTex, Vec2f(472.0f, 416.0f));

		gl::color(ColorA(1.0f, 1.0f, 1.0f, startCounterAlpha));
		gl::draw(*counterPtr, Vec2f(894.0f, 248.0f));
		gl::color(Color::white());

		//drawTitle();
		//drawPreview();
	}

	void drawTitle()
	{			
		gl::pushMatrices();
		gl::translate(780.0f, 107.0f);
		//gl::draw(titleNum);
		gl::translate(262.0f, 9.0f);
		gl::draw(levelNumTexture);
		gl::popMatrices();
	}

	void drawPreview(bool isShadow = true)
	{	
		//gl::draw(iconsPreview, Vec2f(19.0f, 15.0f));

		/*for (int i = 0; i < gameLevel - 1; i++)
		{
			gl::pushMatrices();
			gl::translate(previewVec[i].position);
			gl::scale(previewVec[i].scale);
			gl::rotate(previewVec[i].rotation);
			gl::draw(previewVec[i].screenshot);
			gl::popMatrices();
		}*/
	}

	void init()
	{
		state = SHOW_STEP_BACK;
		bubbleAlpha = 0.0f;
		timeline().apply( &bubbleAlpha, 0.0f, 1.0f, 0.8f, EaseInCubic() );
		bubbleScale = Vec2f(0.7f, 0.7f);
		timeline().apply( &bubbleScale,  Vec2f(1.0f,1.0f), 0.7f, EaseOutCubic() );

		catVec = Vec2f(78.0f, -414.0f);
		timeline().apply( &catVec,  Vec2f(78.0f, 0.0f), 0.7f, EaseOutCubic() );

		arrowAlpha = 0.0f;
		timeline().apply( &arrowAlpha, 0.0f, 1.0f, 0.8f, EaseInCubic() );

		arrowScale = Vec2f(1.0f, 1.0f);
		sign = 1;

		arrowVec = Vec2f(901.0f, 442.0f);
		timeline().apply( &arrowVec,  Vec2f(901.0f, 472.0f), 0.7f, EaseOutCubic() );
	}

	void startReadySate()
	{
		//screenshot = gl::Texture(copyWindowSurface());
		timeline().apply( &bgAlpha, 0.0f, 1.0f, 0.7f, EaseInCubic());
		//if (poseNum == 1)
		//{
		//	timeline().apply( &alphaBg, 0.0f, 1.0f, 0.7f, EaseInCubic() );
		//	//levelNumTexture = Utils::getTextField(to_string(poseNum), &levelNumFont, Color::black());
		//	
		//}
		//else
		//{
		//	previewVec[poseNum-2].screenshot = screenshot;
		//	timeline().apply( &scaleComicsAnimate,    Vec2f(1.0f, 1.0f), previewVec[poseNum-2].scale, 0.4f, EaseInCubic());
		//	timeline().apply( &positionComicsAnimate, Vec2f(0.0f, 0.0f), previewVec[poseNum-2].position, 0.4f, EaseInCubic());
		//	timeline().apply( &rotationComicsAnimate, 0.0f             , previewVec[poseNum-2].rotation, 0.4f, EaseInCubic());

		//	alphaBg = 1.0f;
		//	//levelNumTexture = Utils::getTextField(to_string(poseNum), &levelNumFont, Color::black());
		//}
		state = SHOW_READY;
	}

	void startHandsAwaiting()
	{
		state = HANDS_AWAITING;

		timeline().apply( &hintAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic() );
		timeline().apply( &manHandsUpTexAnimateVec, Vec2f(378.0f, 27.0f), Vec2f(378.0f, 127.0f), 0.4f, EaseOutCubic() );
		timeline().apply( &handsUpTextTexAnimateVec, Vec2f(1190.0f, 328.0f), Vec2f(990.0f, 328.0f), 0.4f, EaseOutCubic() );
	}

	void startCountDown()
	{	
		timeline().apply( &readyTextAlpha, 1.0f, 0.0f, 0.2f, EaseInCubic() );

		startCounterAlpha = 0.0f;
		timeline().apply( &startCounterAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic() ).delay(0.1f);

		//countDownTexture = Utils::getTextField("3", &countDownFont, Color::white());
		counterPtr	= &counter3Tex;
		state = SHOW_NUMS;
	}

	void updateCountDown(int time)
	{
		if (time == 1)
		{
			counterPtr	= &counter1Tex;
		}
		else if (time == 2)
		{
			counterPtr	= &counter2Tex;
		}
		else if (time == 3)
		{
			counterPtr	= &counter3Tex;
		}
	}

	void fadeCounter()
	{
		state = FADE_NUMS;
		timeline().apply( &bgAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic() ).finishFn( [ & ]( )
		{
			state = PREVIEW_ONLY;
		});
	}

	void startHint()
	{
		state = START_HINT;
		timeline().apply( &bgAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic());
		hintAlpha = 0.0f;
		timeline().apply( &hintAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic() ).delay(0.45f);
	}

	void fadeHint()
	{
		timeline().apply( &hintAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic() ).finishFn( [ & ]( )
		{
			state = PREVIEW_ONLY;
		});
	}
};

inline HintScreen&	hintScreen() { return HintScreen::getInstance(); };