#pragma once

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>

#include "cinder/Exception.h"
#include "cinder/Surface.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"



namespace ph { namespace awesomium {

// exceptions
class EmptyPointerException : public ci::Exception {
	virtual const char * what() const throw() { return "The specified pointer is empty."; }
};

class InvalidBufferException : public ci::Exception {
	virtual const char * what() const throw() { return "The specified buffer is empty or not initialized."; }
};

// A helper macro, used in 'getWebKeyFromKeyEvent'
#define mapKey(a, b) case ci::app::KeyEvent::KEY_##a: return Awesomium::KeyCodes::AK_##b;

// Translates a Cinder virtual key code to an Awesomium key code
int getWebKeyFromKeyEvent( ci::app::KeyEvent event );

int getKeyCodeRussian(int letter );


// Conversion functions
ci::Surface toSurface( Awesomium::BitmapSurface* surface );

inline ci::Surface toSurface( Awesomium::WebView* webview )
{
	return toSurface( (Awesomium::BitmapSurface*) webview->surface() );
}

ci::gl::Texture toTexture( Awesomium::BitmapSurface* surface, ci::gl::Texture::Format format=ci::gl::Texture::Format() );

inline ci::gl::Texture toTexture( Awesomium::WebView* webview, ci::gl::Texture::Format format=ci::gl::Texture::Format() )
{
	return toTexture( (Awesomium::BitmapSurface*) webview->surface(), format );
}

inline bool isDirty( Awesomium::WebView* webview ) 
{
	if( ! webview ) throw EmptyPointerException();

	Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*) webview->surface();
	if( ! surface ) return false; //throw EmptyPointerException();

	return surface->is_dirty();
}

Awesomium::WebKeyboardEvent toKeyEvent( ci::app::KeyEvent event, Awesomium::WebKeyboardEvent::Type type , bool isImmitate = false, char letter = ' ');

Awesomium::WebKeyboardEvent toKeyChar( ci::app::KeyEvent event ,  bool isImmitate = false, char letter = ' ');

// Utility functions that take care of event handling

//! sends a Cinder KeyDown event to the WebView and handles Cut, Copy and Paste
inline void handleKeyDown( Awesomium::WebView *view, ci::app::KeyEvent event , bool isImmitate = false, char letter = ' ')
{
	// handle cut, copy, paste (as suggested by Simon Geilfus - thanks mate)
	if( event.isAccelDown() )
	{
		switch( event.getCode() )
		{
		case ci::app::KeyEvent::KEY_x: view->Cut(); return;
		case ci::app::KeyEvent::KEY_c: view->Copy(); return;
		case ci::app::KeyEvent::KEY_v: view->Paste(); return;
		}
	}

	// other keys
	view->Focus();
	view->InjectKeyboardEvent( toKeyEvent( event, Awesomium::WebKeyboardEvent::kTypeKeyDown , isImmitate, letter ) );
	view->InjectKeyboardEvent( toKeyChar( event , isImmitate, letter) );
}

//! sends a Cinder KeyUp event to the WebView
inline void handleKeyUp( Awesomium::WebView *view, ci::app::KeyEvent event )
{	
	view->Focus();
	view->InjectKeyboardEvent( toKeyEvent( event, Awesomium::WebKeyboardEvent::kTypeKeyUp ) );
}

//! sends a Cinder MouseMove event to the WebView
inline void handleMouseMove( Awesomium::WebView *view, ci::app::MouseEvent event )
{
	//view->InjectMouseMove( event.getX(), event.getY() );
}

//! sends a Cinder MouseDown event to the WebView
inline void handleMouseDown( Awesomium::WebView *view, ci::app::MouseEvent event )
{
	if( event.isLeft() )
	{
		view->InjectMouseMove( event.getX(), event.getY() );
		view->InjectMouseDown( Awesomium::kMouseButton_Left );
	}
	else if( event.isMiddle() )
		view->InjectMouseDown( Awesomium::kMouseButton_Middle );
	else if( event.isRight() )
		view->InjectMouseDown( Awesomium::kMouseButton_Right );
}

//! sends a Cinder MouseDrag event to the WebView
inline void handleMouseDrag( Awesomium::WebView *view, ci::app::MouseEvent event )
{
	view->InjectMouseMove( event.getX(), event.getY() );
}

//! sends a Cinder MouseUp event to the WebView
inline void handleMouseUp( Awesomium::WebView *view, ci::app::MouseEvent event )
{
	if( event.isLeft() )
	{
		view->InjectMouseMove( event.getX(), event.getY() );
		view->InjectMouseUp( Awesomium::kMouseButton_Left );
	}
	else if( event.isMiddle() )
		view->InjectMouseUp( Awesomium::kMouseButton_Middle );
	else if( event.isRight() )
		view->InjectMouseUp( Awesomium::kMouseButton_Right );
}

//! sends a Cinder MouseWheel event to the WebView
inline void handleMouseWheel( Awesomium::WebView *view, ci::app::MouseEvent event, int increment=150 )
{
	view->InjectMouseWheel( increment * int( event.getWheelIncrement() ), 0 );
}

} } // namespace ph::awesomium