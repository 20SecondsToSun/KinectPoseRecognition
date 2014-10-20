#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

using namespace ci;

class Toucher
{
	public:	

		static Toucher& getInstance() { static Toucher toucher; return toucher; };

		void	setPosition(ci::Vec2i _position)
		{
			position = _position;
			timeline().apply( &alphaAnimate, 0.7f, 0.0f, 0.5f, EaseOutCubic() );
			timeline().apply( &sizeAnimate, 10.f, 30.0f, 0.5f, EaseOutCubic() );
		}

		void draw()
		{
			gl::color(ColorA(1, 1, 1, alphaAnimate));
			gl::drawSolidCircle(position, sizeAnimate, 16);
			gl::color(ColorA(1, 1, 1, 1));
		}

	private :

		ci::Vec2i	position;
		ci::Anim<float> alphaAnimate, sizeAnimate;	
};

inline Toucher&	toucher() { return Toucher::getInstance(); };