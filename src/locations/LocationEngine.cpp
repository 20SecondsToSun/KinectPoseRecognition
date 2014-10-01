#include "LocationEngine.h"
#include "Location.h"

void LocationEngine::init(const char* title, ci::app::WindowRef window , int width, int height, 
						 bool fullscreen)
{
	mainWindow = window;
	isAnimation = false;

	l_MouseDown = mainWindow->getSignalMouseDown().connect( std::bind( &LocationEngine::mouseDown, this,std::placeholders::_1) );
	l_KeyDown   = mainWindow->getSignalKeyDown().connect( std::bind( &LocationEngine::keyDown, this,std::placeholders::_1) );


	//mCbMouseDrag = mWindow->getSignalMouseDrag().connect( std::bind( &SquareListener::mouseDrag, this, std::_1 ) );	
}

void LocationEngine::mouseDown( MouseEvent &event )
{	
	if (isAnimation) return;
	
	mouseEvent = event;	
	states.back()->mouseEvents();
}

void LocationEngine::keyDown( KeyEvent &event )
{	
	if (isAnimation) return;
	
	keyEvent = event;	
	states.back()->keyEvents();
}

void LocationEngine::cleanup()
{
	// cleanup the all states
	while ( !states.empty() )
	{
		states.back()->cleanup();
		states.pop_back();
	}	
}

void LocationEngine::changeState(Location* state) 
{
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->cleanup();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->init(this);
}

void LocationEngine::pushState(Location* state)
{
	// pause current state
	if ( !states.empty() ) {
		states.back()->pause();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->init(this);
}

void LocationEngine::popState()
{
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->cleanup();
		states.pop_back();
	}

	// resume previous state
	if ( !states.empty() ) {
		states.back()->resume();
	}
}

void LocationEngine::handleEvents() 
{
	// let the state handle events
	states.back()->handleEvents();
}

void LocationEngine::update() 
{
	// let the state update the game
	states.back()->update();
}

void LocationEngine::draw() 
{
	// let the state draw the screen
	states.back()->draw();
}