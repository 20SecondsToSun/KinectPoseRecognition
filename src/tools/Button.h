#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"
#include <boost/signals2.hpp>
#include "boost/bind.hpp"

using namespace std;

class Button
{
	public:

		void			setup(ci::gl::Texture _tex,ci::Font _font, std::string _char, bool _isText  = true);
		void			setup(ci::gl::Texture _tex, std::string _char);
		void			setup(ci::Rectf rect, ci::Color color);

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

		boost::signals2::signal<void(void)> mouseDownSignal;
		void addEventListener( ci::app::WindowRef window);
		void removeEventListener();

		
	private:
		ci::gl::Texture textTexture;
		std::string		text;
		ci::Font		font;
		ci::Rectf		field;
		std::string		code;
		bool			isText;

		ci::Anim<ci::Color> overColor;
		ci::Anim<ci::ColorA> overTextColor;

		ci::Color		maincolor;

		ci::signals::connection KeyDownCon, MouseDownCon, MouseUpCon;
		void			MouseDown( ci::app::MouseEvent &event );

};