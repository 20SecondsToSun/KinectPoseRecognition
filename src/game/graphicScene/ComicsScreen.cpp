#include "ComicsScreen.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

void ComicsScreen::setup()
{
	cameraImageScale = 1.8181818f;
	scaleToFit = 1249.0f / 1920.0f;
	failImage = *AssetManager::getInstance()->getTexture("images/diz/mimo.png");
}

void ComicsScreen::createResultComicsFail(int index)
{
	_successComics[index] = failImage;
}

void ComicsScreen::createResultComics(int index)
{
	mainFbo = gl::Fbo(1249, 703);
	currentSuccessIndex = index;

	calculateFinalOffset();

	fboWidth = (int)(1920.0f * poseScale);
	fboHeight = (int)(1080.0f * poseScale);

	drawAllGraphicsToFBO();

	PlayerData::setFragmentScaleOptions(fboWidth, fboHeight, poseScale, poseShiftVec);
	PlayerData::finalShift = finalShift;

	_successComics[index] = mainFbo.getTexture();

	Utils::clearFBO(mainFbo);

#ifdef debug
	Utils::printVideoMemoryInfo();
#endif	
}

void ComicsScreen::calculateFinalOffset()
{
	float x1 = middleVec.x * 3 + kinect().viewShiftX;
	float y1 = middleVec.y * 3 + kinect().viewShiftY;

	float x2 = x1 * poseScale;
	float y2 = y1 * poseScale;

	float finX = x1 - x2;
	float finY = y1 - y2;
	finalShift = -Vec2f(finX, finY);
}

void ComicsScreen::drawAllGraphicsToFBO()
{
	Utils::drawGraphicsToFBO(mainFbo, [&]()
	{
		gl::clear(Color::black());
		gl::pushMatrices();
		gl::scale(scaleToFit / poseScale, scaleToFit / poseScale);
		gl::pushMatrices();
		gl::translate(cameraCanon().getSurfaceTranslate());
		gl::translate(finalShift);
		gl::scale(-cameraImageScale, cameraImageScale);
		gl::draw(cameraImage);
		gl::popMatrices();
		gl::popMatrices();

		gl::pushMatrices();
		gl::scale(scaleToFit, scaleToFit);
		gl::draw(comicsImage);
		gl::popMatrices();
	});
}

Texture ComicsScreen::getCurrentComics()
{
	if (guess)
		return _successComics[currentSuccessIndex];

	return failImage;
}

void ComicsScreen::draw()
{
	if (guess)
	{
		float scale = 1920.0f / _successComics[currentSuccessIndex].getWidth();
		gl::pushMatrices();
		gl::scale(scale, scale);
		gl::draw(_successComics[currentSuccessIndex]);
		gl::popMatrices();
	}
	else
		gl::draw(failImage);
}

void ComicsScreen::setPoseScale(float value)
{
	poseScale = value;
}

void ComicsScreen::setPoseShift(const Vec2f& value)
{
	poseShiftVec = value;
}

void ComicsScreen::setMiddlePoint(const Vec2f& value)
{
	middleVec = value;
}

void ComicsScreen::setCameraImage(const Texture& value)
{
	cameraImage = value;
}

void ComicsScreen::setComicsImage(const Texture& value)
{
	comicsImage = value;
}

void ComicsScreen::setGuess(bool value)
{
	guess = value;
}

ci::gl::Texture ComicsScreen::getSuccessComics(int index)
{
	return _successComics[index];
}