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
		ci::Anim<float>	 alphaTint, alphaHint1, alphaHint2, stepBackAlpha;

		ci::Anim<ci::Vec2f>	 hint1Vec;
		ci::Anim<ci::Vec2f>	 hint2Vec;

		Texture hint1, hint2, bg;

		float tint_r, tint_g, tint_b;

		int state;

		void setup()
		{
			hint1   = *AssetManager::getInstance()->getTexture( "images/diz/hint1.png" );
			hint2   = *AssetManager::getInstance()->getTexture( "images/diz/hint2.png" );
			bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );

			tint_r = 1/255.0;
			tint_g = 31/255.0;
			tint_b = 62/255.0;
		}

		void draw()
		{
			if (state == 2 || state == 3)
			{
				gl::color(ColorA(1, 1, 1, stepBackAlpha));
				gl::draw(bg);
				Utils::textFieldDraw("Œ“Œ…ƒ»“≈ œŒƒ¿À‹ÿ≈", fonts().getFont("Helvetica Neue", 66), Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
				gl::color(Color::white());	


				gl::color(ColorA(tint_r, tint_g, tint_b, alphaTint));
				gl::drawSolidRect(Rectf(0,0, getWindowWidth(), getWindowHeight()));
				gl::color(Color::white());

				gl::color(ColorA(1, 1, 1, alphaHint1));
				gl::pushMatrices();
				gl::translate(hint1Vec);
					gl::draw(hint1);
				gl::popMatrices();

				gl::color(ColorA(1, 1, 1, alphaHint1));
				gl::pushMatrices();
				gl::translate(hint2Vec);
					gl::draw(hint2);
				gl::popMatrices();
				gl::color(Color::white());	

				

			}
			else if (state ==1)
			{
				gl::draw(bg);
				gl::color(ColorA(1, 1, 1, stepBackAlpha));
				Utils::textFieldDraw("Œ“Œ…ƒ»“≈ œŒƒ¿À‹ÿ≈", fonts().getFont("Helvetica Neue", 66), Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));
				gl::color(Color::white());	
			}

			
		}

		void init()
		{
			state = 1;
			stepBackAlpha = 0.0f;
			timeline().apply( &stepBackAlpha, 0.0f, 1.0f, 0.9f, EaseOutCubic() );
			
		}

		void startHint()
		{
			if (state == 2) return;

			state = 2;

			timeline().apply( &stepBackAlpha, 0.0f, 0.9f, EaseOutCubic() );


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
			if (state == 3) return;

			state = 3;
			
			timeline().apply( &alphaTint, 0.0f, 0.7f, EaseOutCubic() );		
			timeline().apply( &alphaHint1, 0.0f, 0.7f, EaseOutCubic() );			
			timeline().apply( &alphaHint2, 0.0f, 0.7f, EaseOutCubic() );		
			timeline().apply( &hint1Vec, Vec2f(275, 635), 0.7f, EaseOutCubic() );			
			timeline().apply( &hint2Vec, Vec2f(1046, 347), 0.7f, EaseOutCubic() );
		}



		void update()
		{
			
		}

		void animationFin()
		{
			
		}		

		void kill()
		{
		
		}
};

inline HintScreen&	hintScreen() { return HintScreen::getInstance(); };