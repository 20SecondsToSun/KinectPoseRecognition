#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "CameraAdapter.h"
#include "cinder/Text.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

class ComicsScreen
{
	public:

		static ComicsScreen& getInstance() { static ComicsScreen game; return game; };
	
		bool isGuess;
		Texture comicsTexture, poseTexture, failImage, bg_blue;
		Anim<float> alphaAnim;

		float poseScale;
		Vec2f poseShiftVec;

		Fbo mFbo;

		void setup()
		{
			failImage    = *AssetManager::getInstance()->getTexture( "images/fail.jpg" );
			bg_blue		 = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );
			alphaAnim = 0;
			isGuess = false;
		}

		void show()
		{
			//timeline().apply( &alphaAnim, 0.0f, 1.0f, 0.9f, EaseOutCubic());

			int fboWidth =  (int)(1920.0f * poseScale);
			int fboHeight =  (int)(1080.0f * poseScale);
			mFbo = gl::Fbo(fboWidth, fboHeight);
			drawToFBO();

			Params::saveBufferSuccessComics(mFbo.getTexture());
		}

		void draw()
		{
			if (isGuess)
			{
				drawPoseComics();
			}
			else
			{
				gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnim));
				gl::draw(failImage);
				gl::color(Color::white());
			}
		}

		void setPoseScale(float  _poseScale)
		{
			poseScale = _poseScale;
		}

		void setPoseShift(Vec2f  _poseShiftVec)
		{
			poseShiftVec = _poseShiftVec;
		}

		void drawPoseComics()
		{
			gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnim));
				gl::draw(bg_blue);
			gl::color(Color::white());
			
			/*gl::pushMatrices();
				gl::translate(cameraCanon().getSurfaceTranslate());
				gl::scale(-cameraCanon().scaleFactor, cameraCanon().scaleFactor);
				gl::draw(comicsTexture);
			gl::popMatrices();*/

			

			


			gl::pushMatrices();
				gl::translate(poseShiftVec * 3);
				gl::draw(mFbo.getTexture());
				//gl::scale(poseScale, poseScale);
				//gl::draw(comicsTexture);
				//gl::draw(poseTexture);
				//gl::translate(poseShiftVec);
			gl::popMatrices();

			//float x1 = poseShiftVec.x * 3;// * poseScale;
			//float y1 = poseShiftVec.y * 3;// * poseScale;

			//gl::drawStrokedRect(Rectf(Area(x1, y1, x1 + 1920.0f * poseScale, y1 + 1080.0f * poseScale)));
		}

		void drawToFBO()
		{
			gl::SaveFramebufferBinding bindingSaver;
			mFbo.bindFramebuffer();
			Area saveView = getViewport();
			gl::setViewport(mFbo.getBounds());

			gl::pushMatrices();
			gl::setMatricesWindow( mFbo.getSize(), false);
			gl::clear( Color::black());	
				gl::pushMatrices();
					gl::translate(cameraCanon().getSurfaceTranslate());
					gl::translate(-poseShiftVec * 3);
					gl::scale(-cameraCanon().scaleFactor, cameraCanon().scaleFactor);
					gl::draw(comicsTexture);
				gl::popMatrices();
		
				gl::scale(poseScale, poseScale);
				gl::draw(poseTexture);
			
			gl::popMatrices();

			gl::setViewport(saveView);
		}
};

inline ComicsScreen&	comicsScreen() { return ComicsScreen::getInstance(); };