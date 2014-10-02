#include "ButtonTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MouseEvents;


ButtonTexture::ButtonTexture(ci::gl::Texture _tex)
{
	texture = _tex;	
	isTextField = false;
}

ButtonTexture::ButtonTexture(ci::gl::Texture _tex, string _char)
{
	texture = _tex;		
	code = _char;
	overColor = Color::white();
	isTextField = false;
}

ButtonTexture::ButtonTexture(ci::gl::Texture _tex, Font* _font, string _char)
{
	texture = _tex;
	label = _char;
	textFont = _font;
	isTextField = true;
	code = _char;

	overColor = Color::white();

	createTextField();	
}

void ButtonTexture::draw()
{	
	gl::pushMatrices();
		gl::translate(field.x1, field.y1);	
		gl::color(overColor);
		gl::draw(texture);	
		gl::color(Color::black());			
	gl::popMatrices();	
}
