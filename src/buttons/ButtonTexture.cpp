#include "ButtonTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MouseEvents;

ButtonTex::ButtonTex(ci::gl::Texture _tex, string _char)
{
	animate = false;
	texture = _tex;		
	code = _char;
	overColor = Color::white();
	isTextField = false;
	isDown = false;
	MouseDownCon = getWindow()->getSignalMouseDown().connect( std::bind( &ButtonTex::MouseDown, this, std::placeholders::_1 ) );
	MouseUpCon   = getWindow()->getSignalMouseUp().connect( std::bind( &ButtonTex::MouseUp, this, std::placeholders::_1 ) );
}

ButtonTex::ButtonTex(ci::gl::Texture _tex, Font* _font, string _char)
{
	animate = false;
	texture = _tex;
	label = _char;
	textFont = _font;
	isTextField = true;
	code = _char;
	isDown = false;
	overColor = Color::white();

	createTextField();	
}

void ButtonTex::setDownState(ci::gl::Texture _tex, ci::Vec2f vec)
{
	downTexture = _tex;
	donwShiftVec = vec;
}

void ButtonTex::MouseDown( MouseEvent &event )
{
	if( contains(event.getPos()))
	{
		isDown = true;
		mouseDownEvent();
	}
}

void ButtonTex::MouseUp( MouseEvent &event )
{
	isDown = false;
	if( contains(event.getPos()))
	{
		mouseUpEvent();
	}
}

void ButtonTex::draw()
{	
	gl::pushMatrices();
		gl::translate(field.x1, field.y1);
			
		if (isDown)
		{
			if(downTexture)
			{
				gl::pushMatrices();
					gl::translate(donwShiftVec);
					gl::draw(downTexture);	
				gl::popMatrices();
			}
			else
			{	
				gl::color(overColor);
				gl::draw(texture);
			}
		}
		else
			gl::draw(texture);	

		//gl::color(Color::white());
		if (isTextField)
		{
			gl::pushMatrices();
			gl::translate((texture.getWidth()-textTexture.getWidth())*0.5f,(texture.getHeight()-textTexture.getHeight())*0.5f - 5.0f);
			gl::draw(textTexture);
			gl::popMatrices();
		}
	gl::popMatrices();
}