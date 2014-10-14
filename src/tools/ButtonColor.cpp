#include "ButtonColor.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MouseEvents;

ButtonColor::ButtonColor(ci::Rectf rect, ci::Color color)
{
	field = rect;	
	maincolor = color;
	overColor = Color::white();
	isTextField = false;
}

ButtonColor::ButtonColor(ci::Rectf rect, ci::Color color, ci::Font* font, string _label)
{
	field = rect;	
	maincolor = color;
	overColor = Color::white();
	isTextField = true;
	textFont = font;
	label = _label;

	createTextField();	
}

ButtonColor::ButtonColor(ci::app::WindowRef window, ci::Rectf rect, ci::Color color, ci::Font* font, string _label)
{
	field = rect;	
	maincolor = color;
	overColor = Color::white();
	isTextField = true;
	textFont = font;
	label = _label;

	createTextField();	

	windowref = window;
	MouseDownCon   = window->getSignalMouseDown().connect( std::bind( &Button::MouseDown, this, std::placeholders::_1 ) );
	//mouseDownEvent = new ButtonSignal();

	//console()<<" setup btton ::::::::::: "<<endl;
}




void ButtonColor::draw()
{	
	gl::pushMatrices();
		gl::color(maincolor);
		gl::drawSolidRect(field);
	gl::popMatrices();

	if (isTextField)
	{		
		gl::pushMatrices();
		gl::translate(field.x1, field.y1);
		gl::color(Color::white());
		gl::draw(textTexture);
		gl::popMatrices();
	}
}
