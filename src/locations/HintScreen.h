#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "AssetsManager.h"

using namespace ci;
using namespace gl;

class HintScreen
{
	public:	

		static HintScreen& getInstance() { static HintScreen game; return game; };

		ci::Timer timer;
		int index;
		ci::Anim<float>	 alphaTint, alphaHint1, alphaHint2, bubbleAlpha, arrowAlpha, alphaBg, readyTextAlpha, startCounterAlpha, lastAlpha;

		ci::Anim<ci::Vec2f>	 hint1Vec;
		ci::Anim<ci::Vec2f>	 hint2Vec;

		ci::Anim<ci::Vec2f>	 catVec, arrowVec, arrowScale, bubbleScale;


		Texture hint1, hint2, bg, catImage, bubbleImage, arrowImage, screenshot, readyTex, titleNum, countDownTexture, levelNumTexture;
		
		float tint_r, tint_g, tint_b;

		int _state;

		ci::Font countDownFont, levelNumFont;

		int poseNum;

		enum
		{
			SHOW_STEP_BACK,
			SHOW_TIMER,
			SHOW__COUNTDOWN,
			SHOW_NUMS,
			SHOW_READY,
			SHOW_HINT
		};

		void setup()
		{
			hint1   = *AssetManager::getInstance()->getTexture( "images/diz/hint1.png" );
			hint2   = *AssetManager::getInstance()->getTexture( "images/diz/hint2.png" );
			bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );

			catImage = *AssetManager::getInstance()->getTexture( "images/diz/cat3.png" );
			bubbleImage = *AssetManager::getInstance()->getTexture( "images/diz/bubble2.png" );
			arrowImage = *AssetManager::getInstance()->getTexture( "images/diz/arrow2.png" );

			readyTex= *AssetManager::getInstance()->getTexture( "images/diz/readyText.png" );
			titleNum= *AssetManager::getInstance()->getTexture( "images/diz/pozaTitle.png" );

			countDownFont= Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 650);
			levelNumFont= Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 63);

			tint_r = 1/255.0;
			tint_g = 31/255.0;
			tint_b = 62/255.0;
		}

		void draw()
		{
			//if (state == 2 || state == 3)
			//{
			//	//gl::color(ColorA(1, 1, 1, stepBackAlpha));
			//	gl::draw(bg);
			//	//Utils::textFieldDraw("Œ“Œ…ƒ»“≈ œŒƒ¿À‹ÿ≈", fonts().getFont("Helvetica Neue", 66), Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
			//	//gl::color(Color::white());	


			//	gl::color(ColorA(tint_r, tint_g, tint_b, alphaTint));
			//	gl::drawSolidRect(Rectf(0,0, getWindowWidth(), getWindowHeight()));
			//	gl::color(Color::white());

			//	gl::color(ColorA(1, 1, 1, alphaHint1));
			//	gl::pushMatrices();
			//	gl::translate(hint1Vec);
			//		gl::draw(hint1);
			//	gl::popMatrices();

			//	gl::color(ColorA(1, 1, 1, alphaHint1));
			//	gl::pushMatrices();
			//	gl::translate(hint2Vec);
			//		gl::draw(hint2);
			//	gl::popMatrices();
			//	gl::color(Color::white());

			//}
			//else
			
			if (_state == SHOW_STEP_BACK)
			{
				gl::draw(bg);
				gl::color(ColorA(1, 1, 1, bubbleAlpha));
				gl::pushMatrices();
					gl::translate(799, 117);
					gl::scale(bubbleScale);
					gl::draw(bubbleImage);
				gl::popMatrices();
				gl::color(Color::white());

				gl::pushMatrices();
					gl::translate(catVec);
					gl::draw(catImage);
				gl::popMatrices();

				gl::color(ColorA(1, 1, 1, arrowAlpha));
				gl::pushMatrices();
					gl::translate(arrowVec);
					gl::scale(arrowScale);
					gl::draw(arrowImage);
				gl::popMatrices();
				gl::color(Color::white());	
			}
			else if (_state == SHOW_READY)
			{

				gl::draw(screenshot);			
				gl::color(ColorA(235/255.0, 237/255.0,238/255.0, alphaBg));				
				gl::drawSolidRect(Rectf( 0.0f, 0.0f, getWindowWidth(), getWindowHeight()));				
				gl::pushMatrices();
					gl::translate(472, 416);
					gl::draw(readyTex);
				gl::popMatrices();

				 drawTitle();

				gl::color(Color::white());				
			}
			else if (_state == SHOW_NUMS)
			{		
				
				gl::color(ColorA(235/255.0, 237/255.0, 238/255.0, 1));				
				gl::drawSolidRect(Rectf( 0.0f, 0.0f, getWindowWidth(), getWindowHeight()));	

				gl::pushMatrices();
					gl::translate(472, 416);
					gl::color(ColorA(1, 1, 1, readyTextAlpha));		
					gl::draw(readyTex);
				gl::popMatrices();
				
				gl::pushMatrices();
					gl::translate(780, 222);
					gl::color(ColorA(196/255.0,47/255.0,57/255.0, startCounterAlpha));
					gl::color(Color::hex(0xc42f39));
					gl::draw(countDownTexture);
				gl::popMatrices();
				gl::color(Color::white());	

				drawTitle();
				
			}	
			else if (_state == SHOW_HINT)
			{
				gl::color(ColorA(1, 1, 1, lastAlpha));
				gl::draw(screenshot);		
				gl::color(Color::white());		
			}

		}

		void drawTitle()
		{			
			gl::pushMatrices();
				gl::translate(780, 107);
				gl::draw(titleNum);
				gl::translate(302, 9);
				gl::draw(levelNumTexture);
			gl::popMatrices();
		}

		void init()
		{
			_state = SHOW_STEP_BACK;
			bubbleAlpha = 0.0f;
			timeline().apply( &bubbleAlpha, 0.0f, 1.0f, 0.8f, EaseInCubic() );	
			bubbleScale = Vec2f(0.7f, 0.7f);
			timeline().apply( &bubbleScale,  Vec2f(1.0f,1.0f), 0.7f, EaseOutCubic() );	

			catVec = Vec2f(78.0f, -414.0f);
			timeline().apply( &catVec,  Vec2f(78.0f, 0.0f), 0.7f, EaseOutCubic() );	

			arrowAlpha = 0.0f;
			timeline().apply( &arrowAlpha, 0.0f, 1.0f, 0.8f, EaseInCubic() );	

			arrowScale = Vec2f(1.0f, 1.0f);

			arrowVec = Vec2f(701.0f, 452.0f);
			timeline().apply( &arrowVec,  Vec2f(701.0f, 472.0f), 0.7f, EaseOutCubic() );
		}

		void startReadySate()
		{
			screenshot = gl::Texture(copyWindowSurface());
			alphaBg = 0.0f;
			timeline().apply( &alphaBg, 0.0f, 1.0f, 0.9f, EaseInCubic() );

			levelNumTexture = Utils::getTextField(to_string(poseNum), &levelNumFont, Color::white());
			
			_state = SHOW_READY;
		}

		void startCountDown()
		{
			readyTextAlpha = 1.0f;
			timeline().apply( &readyTextAlpha, 1.0f, 0.0f, 0.2f, EaseInCubic() );

			startCounterAlpha = 0.0f;
			timeline().apply( &startCounterAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic() ).delay(0.1f);
			countDownTexture = Utils::getTextField("3", &countDownFont, Color(1,1,1));
			_state = SHOW_NUMS;			
		}
		
		void updateCountDown(int time)
		{		
			countDownTexture = Utils::getTextField(to_string(time), &countDownFont, ColorA(196/255.0,47/255.0,57/255.0));
		}
		
		void startHint()
		{					
			lastAlpha = 1.0f;
			timeline().apply( &lastAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic() );
			screenshot = gl::Texture(copyWindowSurface());
			_state = SHOW_HINT;	

			return;



			if (_state == SHOW_TIMER) return;

			_state = SHOW_TIMER;

			//timeline().apply( &stepBackAlpha, 0.0f, 0.9f, EaseOutCubic() );


			float del = 0.25;

			alphaTint  = 0.78f;
			//timeline().apply( &alphaTint, 0.0f, 0.78f, 0.7f, EaseOutCubic() ).delay(del);

			alphaHint1  = 0.0f;
			timeline().apply( &alphaHint1, 0.0f, 1.0f, 1.0f, EaseOutCubic() ).delay(del);

			alphaHint2  = 0.0f;
			timeline().apply( &alphaHint2, 0.0f, 1.0f, 1.0f, EaseOutCubic() ).delay(del);

			hint1Vec = Vec2f(275, 395);
			timeline().apply( &hint1Vec, Vec2f(275, 455), 1.0f, EaseOutCubic() ).delay(del);

			hint2Vec = Vec2f(1046, 7);
			timeline().apply( &hint2Vec, Vec2f(1046, 177), 1.0f, EaseOutCubic() ).delay(del);

			/*alphaTint  = 0.78f;
			timeline().apply( &alphaTint, 0.0f, 0.7f, EaseOutCubic() );

			alphaHint1  = 1.0f;
			timeline().apply( &alphaHint1, 0.0f, 0.7f, EaseOutCubic() );

			alphaHint2  = 1.0f;
			timeline().apply( &alphaHint2, 0.0f, 0.7f, EaseOutCubic() );*/			
		}

		void out()
		{
			if (_state == 3) return;

			_state = 3;
			
			timeline().apply( &alphaTint, 0.0f, 0.7f, EaseOutCubic() );		
			timeline().apply( &alphaHint1, 0.0f, 0.7f, EaseOutCubic() );			
			timeline().apply( &alphaHint2, 0.0f, 0.7f, EaseOutCubic() );		
			timeline().apply( &hint1Vec, Vec2f(275, 635), 0.7f, EaseOutCubic() );			
			timeline().apply( &hint2Vec, Vec2f(1046, 347), 0.7f, EaseOutCubic() );
		}



		

		void animationFin()
		{
			
		}		

		void kill()
		{
		
		}
};

inline HintScreen&	hintScreen() { return HintScreen::getInstance(); };