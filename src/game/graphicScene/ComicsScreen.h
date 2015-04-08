#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "CameraAdapter.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

class ComicsScreen
{
public:

	static ComicsScreen& getInstance() { static ComicsScreen game; return game; };
	boost::signals2::signal<void(void)> errorFBO;

	void setup()
	{			
		cameraImageScale = 1.8181818f;
		scaleToFit = 1249.0f / 1920.0f;
		failImage  = *AssetManager::getInstance()->getTexture("images/diz/mimo.png");
	}
	
	void createResultComicsFail(int index)
	{
		_successComics[index] = failImage;
	}

	bool createResultComics(int index)
	{
		mainFbo = gl::Fbo(1249, 703);
		currentSuccessIndex = index;

		calculateFinalOffset();

		fboWidth  = (int)(1920.0f * poseScale);
		fboHeight = (int)(1080.0f * poseScale);

		try
		{
			drawAllGraphicsToFBO();
		}
		catch(...)
		{
			errorFBO();
			return false;
		}		

		PlayerData::setFragmentScaleOptions(fboWidth, fboHeight, poseScale, poseShiftVec);		
		PlayerData::finalShift = finalShift;	
		
		_successComics[index] = mainFbo.getTexture();

		Utils::clearFBO(mainFbo);	
	
#ifdef debug
		Utils::printVideoMemoryInfo();	
#endif
		return true;
	}

	void calculateFinalOffset()
	{
		float x1 = middleVec.x * 3 + kinect().viewShiftX;
		float y1 = middleVec.y * 3 + kinect().viewShiftY;

		float x2 = x1 * poseScale;
		float y2 = y1 * poseScale;

		float finX = x1 - x2;
		float finY = y1 - y2;
		finalShift = -Vec2f(finX, finY);
	}

	void drawAllGraphicsToFBO()
	{
		Utils::drawGraphicsToFBO(mainFbo,[ & ]()
		{
			gl::clear(Color::black());
			gl::pushMatrices();			
				gl::scale( scaleToFit/poseScale, scaleToFit/poseScale);
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

	void setPoseScale(float _poseScale)
	{
		poseScale = _poseScale;
	}

	void setPoseShift(const Vec2f& _poseShiftVec)
	{
		poseShiftVec = _poseShiftVec;
	}

	void setMiddlePoint(const Vec2f& _poseShiftVec)
	{
		middleVec = _poseShiftVec;
	}	

	void setCameraImage(const Texture& tex)
	{
		cameraImage = tex;
	}

	void setComicsImage(const Texture& tex)
	{
		comicsImage = tex;
	}

	void setGuess(bool guess)
	{
		this-> guess = guess;
	}	

	Texture getCurrentComics()
	{
		if (guess)
			return _successComics[currentSuccessIndex];

		return failImage;
	}

	void draw()
	{
		if (guess)
		{
			auto tex = mainFbo.getTexture();
			float scale = 1920.0f / tex.getWidth();
			gl::pushMatrices();
			gl::scale(scale, scale);
			gl::draw(tex);
			gl::popMatrices();
		}
		else
		{
			gl::draw(failImage);
		}
	}

	ci::gl::Texture getSuccessComics(int index)
	{
		return _successComics[index];
	}

private:
	bool guess;
	Fbo mainFbo;	
	int fboWidth, fboHeight;
	int currentSuccessIndex;
	float poseScale,  cameraImageScale;
	float scaleToFit;

	Vec2f poseShiftVec, middleVec, finalShift;
	Texture cameraImage, comicsImage, failImage;

	ci::gl::Texture _successComics[2];	
};

inline ComicsScreen& comicsScreen() { return ComicsScreen::getInstance(); };