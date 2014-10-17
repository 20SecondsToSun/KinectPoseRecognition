#include "ButtonTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MouseEvents;

ButtonTex::ButtonTex(ci::gl::Texture _tex)
{
	texture = _tex;	
	isTextField = false;
}

ButtonTex::ButtonTex(ci::gl::Texture _tex, string _char)
{
	texture = _tex;		
	code = _char;
	overColor = Color::white();
	isTextField = false;

}
ButtonTex::ButtonTex(ci::gl::Texture _tex, string _char, ci::app::WindowRef window)
{
	texture = _tex;		
	code = _char;
	overColor = Color::white();
	isTextField = false;
	MouseDownCon   = window->getSignalMouseDown().connect( std::bind( &Button::MouseDown, this, std::placeholders::_1 ) );
}

ButtonTex::ButtonTex(ci::gl::Texture _tex, Font* _font, string _char)
{
	texture = _tex;
	label = _char;
	textFont = _font;
	isTextField = true;
	code = _char;

	overColor = Color::white();

	createTextField();	
}

void ButtonTex::draw()
{	
	gl::pushMatrices();
		gl::translate(field.x1, field.y1);	
		gl::color(overColor);
		gl::draw(texture);	
		gl::color(Color::black());	
		if (isTextField)
		{
			gl::pushMatrices();
			gl::translate((texture.getWidth()-textTexture.getWidth())*0.5f,(texture.getHeight()-textTexture.getHeight())*0.5f - 5.0f);
			gl::draw(textTexture);
			gl::popMatrices();
		}
	gl::popMatrices();
}