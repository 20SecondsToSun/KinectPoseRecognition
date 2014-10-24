#include "Button.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MouseEvents;

void Button::createTextField()
{
	TextLayout simple;
	simple.setFont( *textFont );	
	simple.setColor( Color::white());
	simple.addLine(Utils::cp1251_to_utf8(label.c_str()));		
	textTexture = gl::Texture( simple.render( true, false ) );	
}

void Button::setup(ci::app::WindowRef window)
{
	//windowref = window;
	//MouseDownCon   = window->getSignalMouseDown().connect( std::bind( &Button::MouseDown, this, std::placeholders::_1 ) );
	//mouseDownEvent = new ButtonSignal();
}

void Button::MouseDown( MouseEvent &event )
{
	if( contains(event.getPos()))
	{
		mouseDownEvent();
	}
}

void Button::MouseUp( MouseEvent &event )
{
	if( contains(event.getPos()))
	{
		mouseUpEvent();
	}
}


void Button::removeConnect(int type)
{
	switch (type)
	{
		case MOUSE_DOWN:
			if (MouseDownCon.connected())
				MouseDownCon.disconnect();
		break;
	}	
}

void Button::addEventListener(int type)
{
	switch (type)
	{
		case MOUSE_DOWN:			
			if (!MouseDownCon.connected())
				MouseDownCon   = windowref->getSignalMouseDown().connect( std::bind( &Button::MouseDown, this, std::placeholders::_1 ) );
		break;
	}	
}

void Button::setScreenField(Vec2f vec)
{
	field = Rectf(vec.x,vec.y,vec.x+texture.getWidth(),vec.y+texture.getHeight());	
}

void Button::down()
{	
	timeline().apply( &overColor,  Color::hex(0x9dc9f6), Color::white() , 0.5f, EaseInBack());
}

void Button::up()
{
	//overColor = Color::white();
}

void Button::draw()
{
	gl::enableAlphaBlending();
	gl::color(Color::white());
}

bool Button::contains(Vec2f mousePoint)
{
	return field.contains(mousePoint);
}

float Button::getY()
{
	return field.y1;
}

float Button::getHeight()
{
	return field.y2-field.y1;
}

string  Button::getBtnId()
{
	return code;
}

void  Button::setBtnId(string value)
{
	if (isTextField ) 
	{
		code = value;
		TextLayout simple;
		simple.setFont( *textFont );	
		simple.setColor( Color::white());
		simple.addLine(Utils::cp1251_to_utf8(value.c_str()));		
		textTexture = gl::Texture( simple.render( true, false ) );	
	}
}