#pragma once
#include "Button.h"
using namespace std;

class ButtonTex: public Button
{
	public:

		ButtonTex(ci::gl::Texture _tex,ci::Font* _font, std::string _label);
		ButtonTex(ci::gl::Texture _tex, std::string _label);		
		ButtonTex(ci::gl::Texture _tex, string _char, ci::app::WindowRef window);
		ButtonTex(ci::gl::Texture _tex);

		void		draw();
};