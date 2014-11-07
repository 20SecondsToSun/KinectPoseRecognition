#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "CameraAdapter.h"
#include "cinder/Text.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

class ComicsScreen
{
	public:

		static ComicsScreen& getInstance() { static ComicsScreen game; return game; };
	
		bool isGuess;
		Texture comicsTexture, poseTexture, failImage;
		Anim<float> alphaAnim;

		void setup()
		{
			failImage    = *AssetManager::getInstance()->getTexture( "images/fail.jpg" );
			alphaAnim = 0;
			isGuess = false;
		}

		void show()
		{
			timeline().apply( &alphaAnim, 0.0f, 1.0f, 0.9f, EaseOutCubic());
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

		void drawPoseComics()
		{
			gl::pushMatrices();
				gl::translate(cameraCanon().getSurfaceTranslate());
				gl::scale(-cameraCanon().scaleFactor, cameraCanon().scaleFactor);
				gl::draw(comicsTexture);
			gl::popMatrices();

			gl::draw(poseTexture);
		}
};

inline ComicsScreen&	comicsScreen() { return ComicsScreen::getInstance(); };