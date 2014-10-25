#pragma once
#include "Button.h"
using namespace std;

class ButtonTex: public Button
{
	public:

		ButtonTex(ci::gl::Texture _tex,ci::Font* _font, std::string _label);
		ButtonTex(ci::gl::Texture _tex, std::string _label);		
		ButtonTex(ci::gl::Texture _tex);

		void setDownState(ci::gl::Texture _tex, ci::Vec2f vec = ci::Vec2f(0.0f, 0.0f));
		void draw();
		void MouseDown( ci::app::MouseEvent &event );
		void MouseUp( ci::app::MouseEvent &event );

	private:
		ci::gl::Texture downTexture;
		ci::Vec2f		donwShiftVec;
		bool isDown;
};