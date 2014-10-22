#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace gl;

class GameControLScreen
{
	public:	

		static GameControLScreen& getInstance() { static GameControLScreen game; return game; };

		ci::Timer timer;
		int index;
		ci::Anim<float>	 alphaBubble;

		Texture sidePlashka, krugPercent, sector, ciferblat, arrow;

		ci::Anim<ci::Vec2f> timerVec;

		void setup()
		{
			sidePlashka   = *AssetManager::getInstance()->getTexture( "images/diz/sidePlashka.png" );
			krugPercent   = *AssetManager::getInstance()->getTexture( "images/diz/krugPercent.png" );


			sector   = *AssetManager::getInstance()->getTexture( "images/diz/sector.png" );
			ciferblat   = *AssetManager::getInstance()->getTexture( "images/diz/ciferblat.png" );
			arrow   = *AssetManager::getInstance()->getTexture( "images/diz/arrow.png" );
		}

		void draw()
		{
			gl::pushMatrices();
				gl::translate(timerVec);
				gl::draw(sector);

				gl::pushMatrices();
					gl::translate(Vec2f(-515.0f*0.5f, 38.0f));
					gl::draw(ciferblat);
				gl::popMatrices();	

				gl::translate(Vec2f(0.0f, 2.0f));
				gl::draw(arrow);

			gl::popMatrices();			
		}

		void init()
		{

		}

		void show()
		{
			timerVec = Vec2f(-300.0f, 1076.0f);
			timeline().apply( &timerVec, Vec2f(0.0f, 776.0f), 0.9f, EaseOutCubic() );
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

inline GameControLScreen&	gameControls() { return GameControLScreen::getInstance(); };