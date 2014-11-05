#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace gl;

class CatAnimator
{
	public:	

		static CatAnimator& getInstance() { static CatAnimator game; return game; };

		static const int ONE_CHANGE_TIME = 6;
		static const int TOTAL_CATS = 5;

		struct cat
		{
			ci::Vec2f  startPos, finPos, curPos;
			ci::gl::Texture texture;
		};

		std::vector<cat> catVector;
		ci::Timer timer;
		int index;
		ci::Anim<ci::Vec2f>	 curPos;

		void setup()
		{
			for (int i = 0; i < TOTAL_CATS; i++)
			{
				cat item;
				item.finPos = Vec2f(0.0f, 0.0f);
				item.startPos = Vec2f(0.0f, 0.0f);
				item.texture   = *AssetManager::getInstance()->getTexture( "images/diz/introcat/cat"+to_string(i+1)+".png" );
				catVector.push_back(item);
			}

			catVector[0].startPos = Vec2f(-537.0f, 410.0f);
			catVector[0].finPos = Vec2f(0.0f, 410.0f);

			catVector[1].startPos = Vec2f(1253.0f, -537.0f);
			catVector[1].finPos = Vec2f(1253.0f, 0.0f);

			catVector[2].startPos = Vec2f( 1427.0f, 1080.0f);
			catVector[2].finPos = Vec2f( 1427.0f, 583.0f);

			catVector[3].startPos = Vec2f(636.0f, 1080.0f);
			catVector[3].finPos = Vec2f(636.0f, 905.0f);

			catVector[4].startPos = Vec2f(-623.0f, -541.0f);
			catVector[4].finPos = Vec2f(0.0f, 0.0f);
		}

		void draw()
		{
			gl::draw(catVector[index].texture, curPos);
		}

		void init()
		{
			index = 0;
			curPos = catVector[index].startPos;
			timer.start();
			timeline().apply( &curPos, catVector[index].startPos, catVector[index].finPos, 0.9f, EaseInOutQuart() ).delay(0.5f);
		}

		void update()
		{
			if(!timer.isStopped() && timer.getSeconds() > ONE_CHANGE_TIME)
			{
				timer.stop();				
				timeline().apply( &curPos, catVector[index].finPos, catVector[index].startPos, 0.9f, EaseInOutQuart() ).finishFn( bind( &CatAnimator::animationFin, this ) );	
			}			
		}

		void animationFin()
		{
			index = nextIndex();		
			timer.start();	
			curPos = catVector[index].startPos;
			timeline().apply( &curPos, catVector[index].startPos, catVector[index].finPos, 0.9f, EaseInOutQuart() ).delay(0.5f);
		}

		int nextIndex()
		{
			if (++index == TOTAL_CATS) index = 0;
			return index;
		}

		void kill()
		{
			timer.stop();
		}
};

inline CatAnimator&	catAnimator() { return CatAnimator::getInstance(); };