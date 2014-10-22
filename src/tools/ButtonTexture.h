#pragma once
#include "Button.h"
using namespace std;

class ButtonTex: public Button
{
	public:

		ButtonTex(ci::gl::Texture _tex,ci::Font* _font, std::string _label);
		ButtonTex(ci::gl::Texture _tex, std::string _label);		
		ButtonTex(ci::gl::Texture _tex);
		void    setDownState(ci::gl::Texture _tex);

		void	draw();

		void MouseDown( ci::app::MouseEvent &event );
		void MouseUp( ci::app::MouseEvent &event );
	private:
		ci::gl::Texture downTexture;
		bool isDown;
	
};
