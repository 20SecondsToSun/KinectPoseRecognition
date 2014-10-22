#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"
#include <boost/signals2.hpp>
#include "Utils.h"
//#include "boost/bind.hpp"

using namespace std;

namespace MouseEvents
{
	static const int		MOUSE_DOWN = 1;
	static const int		MOUSE_UP   = 2;
}

namespace MouseTypes
{
	static const int		COLOR_BTN = 1;
	static const int		TEXTURE_BTN = 1;
	
}

class Button
{
	public:
		//boost::signals2::signal<void(void)> __mouseDownEvent;
		typedef boost::signals2::signal<void(void )> ButtonSignal;	
		Button(){};

		void			setup(ci::app::WindowRef window);
		void			draw();
		void			down();
		void			up();
		bool			contains(ci::Vec2f mousePoint);


		std::string		getBtnId();
		void			setScreenField(ci::Vec2f vec);
		ci::gl::Texture texture;

		float			getY();
		float			getHeight();

		void			setBtnId(string value);

		
		ButtonSignal	 mouseDownEvent, mouseUpEvent;
		//boost::signals2::signal<void(void)>	mouseUp_Event;
		
		void removeConnect(int type);
		void addEventListener(int type);
		ci::signals::connection KeyDownCon, MouseDownCon, MouseUpCon;
		void MouseDown( ci::app::MouseEvent &event );
		void MouseUp( ci::app::MouseEvent &event );
		ci::app::WindowRef windowref;

	protected:
		ci::Font*		textFont;
		std::string		label;
		ci::Font		font;
		ci::Rectf		field;

		bool			isTextField;
		void			createTextField();
		ci::Anim<ci::Color> overColor;

		std::string		code;
		ci::gl::Texture textTexture;
		ci::Anim<ci::ColorA> overTextColor;

		

};