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


	startInstructionBtn = new ButtonColor(getWindow(), Rectf(1000,700,1600, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"ПОИГРАЙ СО МНОЙ");

	startGameBtn = new ButtonColor(getWindow(), Rectf(1200,700,1600, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"НАЧАТЬ ИГРУ");		

	startInstructionBtn->mouseDownEvent->connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));
	startGameBtn->mouseDownEvent->connect( boost::bind(&IntroScreen::startGameBtnDown, this));

	createComeBackButton();
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
	isPeopleInFrame = false;
	comeBackBtn->addEventListener(MouseEvents::MOUSE_DOWN);
}

void IntroScreen::cleanup()
{
	startInstructionBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	startGameBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	comeBackBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	comeBackTimerStop();
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
		comeBackTimerStart();
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
			#ifdef debug
				debugString = "";
			#endif
		break;

		case SHOW_INVITE:			
			if (isComeBackTimerKinectFired())
			{
				nextState = INIT;
				changeState();
			}
			#ifdef debug
				if (isPeopleInFrame)
				{
					debugString = "Есть кто-то в зоне  ";//+to_string(kinect().getSkeletsInFrame());
				}
				else
				{
					debugString = "Людей нет с зоне, \nвозвращение на главный экран произойдет через : "+to_string(getSecondsToComeBack());
				}
			#endif
		break;

		case SHOW_INSTRUCTION:			
			if (isComeBackTimerTouchFired())
			{
				nextState = INIT;			
				changeState();	
			}
			#ifdef debug
				debugString = "Возвращение на главный экран произойдет через : "+to_string(getSecondsToComeBack());	
			#endif
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
			drawComeBackButton();
		break;

		case SHOW_INSTRUCTION:
			gl::draw( instructionImage, centeredRect);	
			startGameBtn->draw();
			drawComeBackButton();
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
			startInstructionBtn->addEventListener(MouseEvents::MOUSE_DOWN);
			startGameBtn->removeConnect(MouseEvents::MOUSE_DOWN);		
		break;

		case SHOW_INSTRUCTION:			
			startGameBtn->addEventListener(MouseEvents::MOUSE_DOWN);
			startInstructionBtn->removeConnect(MouseEvents::MOUSE_DOWN);
		break;

		case START_GAME:		
			_game->changeState(ResultScreen::Instance());
		break;
	}	
}

void IntroScreen::gotoFirstScreen() 
{
	if (state!= INIT)
	{
		nextState = INIT;		
		changeState();		
		comeBackTimerStop();	
	}
}