#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace gl;
using namespace std;

class BubbleAnimator
{
	public:	

		static BubbleAnimator& getInstance() { static BubbleAnimator game; return game; };

		static const int ONE_CHANGE_TIME = 4;
		static const int TOTAL_BUBBLES   = 2;

		struct bubble
		{
			Vec2f pos;
			Texture texture;
		};
		vector<bubble> bubbleVector;
		Timer timer;
		int   index;
		Anim<float>	 alphaBubble;

		void setup()
		{
			for (int i = 0; i < TOTAL_BUBBLES; i++)
			{
				bubble item;
				item.pos = Vec2f(0.0f, 0.0f);
				item.texture   = *AssetManager::getInstance()->getTexture( "images/diz/bubble"+to_string(1)+".png" );
				bubbleVector.push_back(item);
			}
			bubbleVector[0].pos = Vec2f(44.0f, 72.0f);
			bubbleVector[1].pos = Vec2f(1194.0f, 15.0f);
		}

		void draw()
		{
			gl::pushMatrices();
				gl::translate(bubbleVector[index].pos);
				gl::color(ColorA(1, 1, 1, alphaBubble));
				gl::draw(bubbleVector[index].texture);
				gl::color(Color::white());			
			gl::popMatrices();
		}

		void init()
		{
			index = 0;
			alphaBubble = 0;
			timer.start();
			timeline().apply( &alphaBubble, 0.0f, 1.0f, 0.9f, EaseInOutQuart() ).delay(0.5f);
		}

		void update()
		{
			if(!timer.isStopped() && timer.getSeconds() > ONE_CHANGE_TIME)
			{
				timer.stop();				
				timeline().apply( &alphaBubble, 1.0f, 0.0f, 0.9f, EaseInOutQuart() ).finishFn( bind( &BubbleAnimator::animationFin, this ) );	
			}			
		}

		void animationFin()
		{
			index = nextIndex();
			alphaBubble = 0;
			timer.start();			
			timeline().apply( &alphaBubble, 0.0f, 1.0f, 0.9f, EaseInOutQuart() );
		}

		int nextIndex()
		{
			if (++index == TOTAL_BUBBLES) index = 0;
			return index;
		}

		void kill()
		{
			timer.stop();
		}
};

inline BubbleAnimator&	bubbleAnimator() { return BubbleAnimator::getInstance(); };