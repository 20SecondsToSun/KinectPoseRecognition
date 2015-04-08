#include "HintScreen.h"

using namespace ci;
using namespace gl;
using namespace std;
using namespace poseParams;

HintScreen::Preview::Preview() :_scale(1.0f), photoEnabled(false)
{

}

void HintScreen::Preview::setBackground(const ci::gl::Texture& tex)
{
	this->background = tex;
}

void HintScreen::Preview::setPhoto(const ci::gl::Texture& photo)
{
	this->photo = photo;
	photoEnabled = true;
	_scale = 136.0f / photo.getWidth();
}

void HintScreen::Preview::clearTexture()
{
	photoEnabled = false;
}

void HintScreen::Preview::setPosition(const ci::Vec2f& vec)
{
	this->position = vec;
}

void HintScreen::Preview::draw()
{
	gl::pushMatrices();
	gl::translate(position);
	gl::draw(background);
	if (photoEnabled)
	{
		gl::translate(Vec2f(10.0f, 10.0f));
		gl::scale(_scale, _scale);
		gl::draw(photo);
	}
	gl::popMatrices();	
}

void HintScreen::setup()
{
	hint3 = *AssetManager::getInstance()->getTexture("images/diz/hint3.png");
	bg = *AssetManager::getInstance()->getTexture("images/diz/bg.jpg");
	catImage = *AssetManager::getInstance()->getTexture("images/diz/cat3.png");
	bubbleImage = *AssetManager::getInstance()->getTexture("images/diz/bubble2.png");
	arrowImage = *AssetManager::getInstance()->getTexture("images/diz/arrow2.png");
	readyTex = *AssetManager::getInstance()->getTexture("images/diz/readyText.png");
	bg_blue = *AssetManager::getInstance()->getTexture("images/diz/bg.jpg");

	countDownFont = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 650);
	levelNumFont = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 63);

	manHandsUpTex = *AssetManager::getInstance()->getTexture("images/diz/handsUpMan.png");
	handsUpTextTex = *AssetManager::getInstance()->getTexture("images/diz/handsUpText.png");

	for (int i = 1; i <= 3; i++)	
		counterTexVec.push_back(*AssetManager::getInstance()->getTexture("images/diz/__" + to_string(i) + ".png"));	
	
	pozeNum = *AssetManager::getInstance()->getTexture("images/diz/pozeNum.png");

	Vec2f initVec = Vec2f(25.f, 25.f);

	for (int i = 1; i <= POSE_IN_GAME_TOTAL; i++)
	{
		Preview preview;
		preview.setBackground(*AssetManager::getInstance()->getTexture("images/diz/preview"+to_string(i)+".png"));
		preview.setPosition(initVec + (i-1) * Vec2f(0.0f, 107.f));
		previewVec.push_back(preview);
	}	

	blue.r = 235.0f / 255.0f;
	blue.g = 237.0f / 255.0f;
	blue.b = 238.0f / 255.0f;

	gameLevel = 1;
	sign = 1;
}

void HintScreen::draw()
{
	switch (state)
	{
	case SHOW_STEP_BACK:
		drawStepBackGraphics();
		break;

	case SHOW_READY:
		drawAreYoureReadyGraphics();
		break;

	case HANDS_AWAITING:
		drawHandsUpGraphics();
		break;

	case SHOW_NUMS:
		drawCountDownTimer();
		break;

	case FADE_NUMS:
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 0));
		gl::draw(bg_blue);
		drawTitle();
		gl::color(Color::white());
		drawPreview(false);
		break;

	case PREVIEW_ONLY:
		gl::color(Color::white());
		drawPreview(false);
		break;

	case START_HINT:
		gl::color(Color::white());
		drawPreview(false);
		gl::color(ColorA(1.0f, 1.0f, 1.0f, hintAlpha));
		gl::draw(hint3, Vec2f(1267.0f, 53.0f));
		break;
	}	
}

void HintScreen::drawStepBackGraphics()
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
		sign = -1;	
	else if (arrowScale.x < 0.6f && sign == -1)	
		sign = 1;	

	if (sign == 1)	
		arrowScale += Vec2f(scaleFactor, scaleFactor);	
	else if (sign == -1)	
		arrowScale -= Vec2f(scaleFactor, scaleFactor);	
}

void HintScreen::drawAreYoureReadyGraphics()
{
	gl::color(ColorA(blue.r, blue.g, blue.b, 1.0f));
	gl::draw(bg_blue);

	gl::color(ColorA(blue.r, blue.g, blue.b, bgAlpha));
	gl::draw(readyTex, Vec2f(472.0f, 416.0f));

	drawTitle();
	drawPreview();
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

void HintScreen::drawHandsUpGraphics()
{
	gl::draw(bg);
	gl::color(ColorA(1.0f, 1.0f, 1.0f, hintAlpha));
	gl::draw(manHandsUpTex, manHandsUpTexAnimateVec);
	gl::draw(handsUpTextTex, handsUpTextTexAnimateVec);
	gl::color(Color::white());
}

void HintScreen::drawCountDownTimer()
{
	gl::color(ColorA(blue.r, blue.g, blue.b, 1.0f));
	gl::draw(bg_blue);

	gl::color(ColorA(1.0f, 1.0f, 1.0f, readyTextAlpha));
	gl::draw(readyTex, Vec2f(472.0f, 416.0f));

	gl::color(ColorA(1.0f, 1.0f, 1.0f, startCounterAlpha));
	gl::draw(*counterPtr, Vec2f(894.0f, 316.0f));
	gl::color(Color::white());

	drawTitle();
	drawPreview();
}

void HintScreen::drawTitle()
{
	gl::pushMatrices();
	gl::translate(780.0f, 107.0f);
	gl::draw(pozeNum);
	gl::translate(262.0f, 9.0f);
	gl::draw(levelNumTexture);
	gl::popMatrices();
}

void HintScreen::drawPreview(bool isShadow)
{
	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
		previewVec[i].draw();
}

void HintScreen::init()
{
	state = SHOW_STEP_BACK;
	bubbleAlpha = 0.0f;
	timeline().apply(&bubbleAlpha, 0.0f, 1.0f, 0.8f, EaseInCubic());
	bubbleScale = Vec2f(0.7f, 0.7f);
	timeline().apply(&bubbleScale, Vec2f(1.0f, 1.0f), 0.7f, EaseOutCubic());

	catVec = Vec2f(78.0f, -414.0f);
	timeline().apply(&catVec, Vec2f(78.0f, 0.0f), 0.7f, EaseOutCubic());

	arrowAlpha = 0.0f;
	timeline().apply(&arrowAlpha, 0.0f, 1.0f, 0.8f, EaseInCubic());

	arrowScale = Vec2f(1.0f, 1.0f);
	sign = 1;

	arrowVec = Vec2f(901.0f, 442.0f);
	timeline().apply(&arrowVec, Vec2f(901.0f, 472.0f), 0.7f, EaseOutCubic());

	poseNum = 1;

	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
		previewVec[i].clearTexture();
}

void HintScreen::startReadySate(const ci::gl::Texture& texture)
{
	previewVec[gameLevel - 2].setPhoto(texture);
	startReadySate();
}

void HintScreen::startReadySate()
{
	screenshot = gl::Texture(copyWindowSurface());
	timeline().apply(&bgAlpha, 0.0f, 1.0f, 0.7f, EaseInCubic());

	timeline().apply(&alphaBg, 0.0f, 1.0f, 0.7f, EaseInCubic());
	levelNumTexture = Utils::getTextField(to_string(gameLevel), &levelNumFont, Color::black());

	state = SHOW_READY;
}

void HintScreen::startHandsAwaiting()
{
	state = HANDS_AWAITING;

	timeline().apply(&hintAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic());
	timeline().apply(&manHandsUpTexAnimateVec, Vec2f(378.0f, 27.0f), Vec2f(378.0f, 127.0f), 0.4f, EaseOutCubic());
	timeline().apply(&handsUpTextTexAnimateVec, Vec2f(1190.0f, 328.0f), Vec2f(990.0f, 328.0f), 0.4f, EaseOutCubic());
}

void HintScreen::startCountDown()
{
	timeline().apply(&readyTextAlpha, 1.0f, 0.0f, 0.2f, EaseInCubic());

	startCounterAlpha = 0.0f;
	timeline().apply(&startCounterAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic()).delay(0.1f);

	counterPtr = &counterTexVec[2];
	state = SHOW_NUMS;
}

void HintScreen::updateCountDown(int time)
{
	counterPtr = &counterTexVec[time - 1];
}

void HintScreen::fadeCounter()
{
	state = FADE_NUMS;
	timeline().apply(&bgAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic()).finishFn([&]()
	{
		state = PREVIEW_ONLY;
	});
}

void HintScreen::startHint()
{
	state = START_HINT;
	timeline().apply(&bgAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic());
	hintAlpha = 0.0f;
	timeline().apply(&hintAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic()).delay(0.45f);
}

void HintScreen::fadeHint()
{
	timeline().apply(&hintAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic()).finishFn([&]()
	{
		state = PREVIEW_ONLY;
	});
}