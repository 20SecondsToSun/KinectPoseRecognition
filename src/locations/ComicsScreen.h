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

		ci::Anim<float> alphaAnim;

		void setup()
		{
			failImage   =  loadImage( loadAsset( "images/fail.jpg" ));
			
			//timerVec = Vec2f(-300.0f, 1076.0f);
			//plashkaVec = Vec2f(2100.0f, 0.0f);	
			alphaAnim = 0;
			isGuess = false;
		}
		void show()
		{
			alphaAnim = 0;
			timeline().apply( &alphaAnim, 1.0f, 0.9f, EaseOutCubic() );
		}

		void draw()
		{
			Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );

			if (isGuess)
			{		
				drawPoseComics();
				gl::disableAlphaBlending();
				//Utils::textFieldDraw("œŒ«€ —Œ¬œ¿À» ”–¿!!! ( "+to_string(recognitionGame().RESULT_TIME - (int)recognitionGame()._resultTimer.getSeconds())+" )", &debugFont46, Vec2f(400.f, 400.0f), ColorA(1.f, 0.f, 0.f, 1.f));
				gl::enableAlphaBlending();
		
			}
			else
			{
				gl::color(ColorA(1, 1, 1, alphaAnim));	
				gl::draw(failImage, centeredRect);
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

			gl::pushMatrices();
				gl::translate(getWindowWidth() - 360.0, getWindowHeight() - 512.0);		
				gl::draw(poseTexture);
			gl::popMatrices();
		}

		void init()
		{

		}		
};

inline ComicsScreen&	comicsScreen() { return ComicsScreen::getInstance(); };