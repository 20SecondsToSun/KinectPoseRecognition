#pragma once

#include "Button.h"

using namespace std;

class ButtonTexture: public Button
{
	public:

		ButtonTexture(ci::gl::Texture _tex,ci::Font* _font, std::string _label);
		ButtonTexture(ci::gl::Texture _tex, std::string _label);		
		ButtonTexture(ci::gl::Texture _tex);

		void		draw();

	private:

};