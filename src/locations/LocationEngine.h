#ifndef LOCATIONENGINE_H
#define LOCATIONENGINE_H

#include <vector>
#include "cinder/app/AppNative.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class Location;

class LocationEngine
{
public:

	void init(const char* title, ci::app::WindowRef window, int width=640, int height=480, 
		        bool fullscreen=false);

	void cleanup();

	void changeState(Location* state);
	void pushState(Location* state);
	void popState();

	void handleEvents();
	void mouseDown( ci::app::MouseEvent &event );
	void keyDown( ci::app::KeyEvent &event );
	void update();
	void draw();

	bool running() { return m_running; }
	void quit() { m_running = false; }
	
	ci::app::MouseEvent	getMouseEvent(){ return mouseEvent; }
	ci::app::KeyEvent	getKeyEvent(){ return keyEvent; }

	bool	freezeLocation;

private:
	// the stack of states
	vector<Location*> states;

	bool m_running;
	bool m_fullscreen;

	ci::app::WindowRef				mainWindow;
	ci::signals::scoped_connection	l_MouseDown, l_KeyDown;//, mCbMouseDrag;

	ci::app::MouseEvent				mouseEvent;
	ci::app::KeyEvent				keyEvent;	
};

#endif