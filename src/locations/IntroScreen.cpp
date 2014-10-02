#include "IntroScreen.h"

using namespace ci;
using namespace ci::app;
using namespace instructionDefaults;

IntroScreen IntroScreen::IntroScreenState;

void IntroScreen::setup()
{	
	introImage = *AssetManager::getInstance()->getTexture( "images/intro.png" );
	playImage  = *AssetManager::getInstance()->getTexture( "images/play.jpg" );
	instructionImage  = *AssetManager::getInstance()->getTexture( "images/instruction.jpg" );


	startInstructionBtn = new ButtonColor(Rectf(1000,700,1600, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"ПОИГРАЙ СО МНОЙ");

	startGameBtn = new ButtonColor(Rectf(1200,700,1600, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"НАЧАТЬ ИГРУ");
	
								  
	startInstructionBtn->setup(getWindow());
	startGameBtn->setup(getWindow());
}

void IntroScreen::startInstructionBtnDown()
{
	nextState = SHOW_INSTRUCTION;		
	changeState();
}

void IntroScreen::startGameBtnDown()
{
	nextState = START_GAME;			
	changeState();	
}

void IntroScreen::init( LocationEngine* game)
{	
	_game = game;
	state = INIT;	

	isChangingStateNow = false;
}

void IntroScreen::cleanup()
{
	startInstructionBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	startGameBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	returnTimer.stop();
}

void IntroScreen::pause()
{
	
}

void IntroScreen::resume()
{
	
}

void IntroScreen::mouseEvents()
{	
	MouseEvent event = _game->getMouseEvent();	

	if(_game->isAnimationRunning()) return;	
	
	if (event.isLeftDown())
	{
		if(state == INIT)
		{
			nextState = SHOW_INVITE;			
			changeState();	
		}
		returnTimer.start();
	}
}

void IntroScreen::keyEvents()
{
	#ifndef kinectInUse
		KeyEvent _event = _game->getKeyEvent();

		switch (_event.getChar())
		{       
			case 'z':
				isPeopleInFrame = true;				
			break;

			case 'x':
			    isPeopleInFrame = false;
			break;
		 }
	 #endif
}

void IntroScreen::handleEvents()
{

}

void IntroScreen::update() 
{	
	#ifdef kinectInUse
		isPeopleInFrame = kinect().getSkeletsInFrame()!=0;
	#endif	

	if(isChangingStateNow) return;

	switch (state)
	{
		case INIT:
			if (isPeopleInFrame)
			{
				nextState = SHOW_INVITE;				
				changeState();
			}			
		break;

		case SHOW_INVITE:			
			checkReturnTimer();
		break;

		case SHOW_INSTRUCTION:			
			checkReturnTimer2();
		break;
	}	
}

void IntroScreen::draw() 
{
	gl::enableAlphaBlending();	
	gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
	Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );

	

	switch (state)
	{
		case INIT:			
			drawInitElements();
		break;

		case SHOW_INVITE:
			gl::draw( playImage, centeredRect);	
			startInstructionBtn->draw();
		break;

		case SHOW_INSTRUCTION:
			gl::draw( instructionImage, centeredRect);	
			startGameBtn->draw();
		break;

		case START_GAME:			
		
		break;
	}

	#ifdef debug
		Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	#endif

	if(isChangingStateNow)
	{
		gl::color(ColorA(0, 0, 0, alphaAnimate));
		gl::drawSolidRect(Rectf(0,0, getWindowWidth(), getWindowHeight()));
		gl::color(Color::white());
	}
}

void IntroScreen::drawInitElements() 
{
	Rectf centeredRect = Rectf( 0,0, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );
	gl::draw( introImage, centeredRect);			
			Utils::textFieldDraw("ЗАСТАВОЧКА", 
			fonts().getFont("Helvetica Neue", 46), 
			Vec2f(600.f, getWindowHeight()*0.5f), 
			ColorA(1, 1, 1, 1));
}


void IntroScreen::changeState() 
{
	isChangingStateNow = true;
	timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &IntroScreen::animationFinished, this ) );	
}

void IntroScreen::animationFinished() 
{
	isChangingStateNow = false;
	state = nextState;
	switch (state)
	{
		case INIT:			
			startInstructionBtn->removeConnect(MouseEvents::MOUSE_DOWN);
			startGameBtn->removeConnect(MouseEvents::MOUSE_DOWN);
		break;

		case SHOW_INVITE:	
			startInstructionBtn->mouseDownEvent->connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));

		break;

		case SHOW_INSTRUCTION:			
			startGameBtn->mouseDownEvent->connect( boost::bind(&IntroScreen::startGameBtnDown, this));
			startInstructionBtn->removeConnect(MouseEvents::MOUSE_DOWN);
		break;

		case START_GAME:			
				_game->changeState(MainGameScreen::Instance());
		break;
	}	
}


void IntroScreen::checkReturnTimer() 
{
	if (!returnTimer.isStopped())
	{
		if (isPeopleInFrame == true)
		{
			returnTimer.stop();
		}
		else if(returnTimer.getSeconds() >= Params::comeBackHomeTime)
		{
			returnTimer.stop();
			nextState = INIT;
			changeState();
		}
	}
	else if (isPeopleInFrame == false)
	{
		returnTimer.start();
	}	

	#ifdef debug
		if (returnTimer.isStopped())
		{
			debugString = "Есть кто-то в зоне  ";//+to_string(kinect().getSkeletsInFrame());
		}
		else
		{
			debugString = "Людей нет с зоне, \nвозвращение на главный экран произойдет через : "+to_string((int)(Params::comeBackHomeTime - returnTimer.getSeconds()));
		}
	#endif
}

void IntroScreen::checkReturnTimer2() 
{
	if (!returnTimer.isStopped() && (returnTimer.getSeconds() >= Params::comeBackHomeTime))
	{
		returnTimer.stop();
		nextState = INIT;			
		changeState();		
	}
	else if(returnTimer.isStopped())
	{		
		returnTimer.start();		
	}

	#ifdef debug
		debugString = "Возвращение на главный экран произойдет через : "+to_string((int)(Params::comeBackHomeTime - returnTimer.getSeconds()));	
	#endif
}