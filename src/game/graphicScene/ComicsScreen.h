#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"

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
		mainFbo   = gl::Fbo(1249, 703);
		cameraImageScale = 1.8181818;
		scaleToFit = 1249.0f / 1920.0f;
	}

	bool createResultComics()
	{
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
		Params::saveBufferSuccessComics(mainFbo.getTexture());
	
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

	void setPoseScale(float  _poseScale)
	{
		poseScale = _poseScale;
	}

	void setPoseShift(Vec2f  _poseShiftVec)
	{
		poseShiftVec = _poseShiftVec;
	}

	void setMiddlePoint(Vec2f  _poseShiftVec)
	{
		middleVec = _poseShiftVec;
	}	

	void setCameraImage(Texture tex)
	{
		cameraImage = tex;
	}

	void setComicsImage(Texture tex)
	{
		comicsImage = tex;
	}

private:

	Fbo mainFbo;	
	int fboWidth, fboHeight;
	float poseScale,  cameraImageScale;
	float scaleToFit;

	Vec2f poseShiftVec, middleVec, finalShift;
	Texture cameraImage, comicsImage;	
};

inline ComicsScreen&	comicsScreen() { return ComicsScreen::getInstance(); };