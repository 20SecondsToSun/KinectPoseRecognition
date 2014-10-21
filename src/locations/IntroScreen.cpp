#include "IntroScreen.h"

using namespace ci;
using namespace ci::app;
using namespace colorsParams;

IntroScreen IntroScreen::IntroScreenState;

void IntroScreen::setup()
{	
	cat		  = *AssetManager::getInstance()->getTexture( "images/diz/cat.png" );
	logo	  = *AssetManager::getInstance()->getTexture( "images/diz/logo1.png" );	
	text1	  = *AssetManager::getInstance()->getTexture( "images/diz/text1.png" );	


	playImage		  = *AssetManager::getInstance()->getTexture( "images/play.jpg" );
	instructionImage  = *AssetManager::getInstance()->getTexture( "images/instruction.jpg" );

	Font *btnFont = fonts().getFont("Helvetica Neue", 46);

	startInstructionBtn = new ButtonColor(Rectf(1000,700,1600, 800),  RED, btnFont, "������� �� ����");
	startGameBtn		= new ButtonColor(Rectf(1200,700,1600, 800),  RED, btnFont, "������ ����");	
	comeBackBtn			= new ButtonColor(Rectf(1200,300, 1600, 400), RED, btnFont, "�����");
}

void IntroScreen::init( LocationEngine* game)
{	
	_game = game;
	state = INIT;	

	_game->freezeLocation = false;
	isPeopleInFrame = false;
	
	startInstructionBtnSignal = startInstructionBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));
	startGameBtnSignal		  = startGameBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::startGameBtnDown, this));		
	comeBackBtnSignal		  = comeBackBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::gotoFirstScreen, this));

	catAnimate = -507;
	timeline().apply( &catAnimate, 0.0f, 0.5f, EaseOutCubic() );

	logoAnimate= -1080;
	timeline().apply( &logoAnimate, 0.0f, 0.5f, EaseOutCubic() );

	textAnimateY = 870;
	textAnimateAlpha = 0.6;
	timeline().apply( &textAnimateY, 885.0f, 0.5f, EaseOutCubic() );
	timeline().apply( &textAnimateAlpha, 885.0f, 0.5f, EaseOutCubic() );
}

void IntroScreen::startInstructionBtnDown()
{
	if(!_game->freezeLocation)
	{
		nextState = SHOW_INSTRUCTION;		
		changeState();
	}	
}

void IntroScreen::startGameBtnDown()
{
	if(!_game->freezeLocation)
	{
		nextState = START_GAME;			
		changeState();
	}
}

void IntroScreen::gotoFirstScreen() 
{
	if (state!= INIT && !_game->freezeLocation)
	{
		nextState = INIT;		
		changeState();		
		comeBackTimerStop();	
	}
}

void IntroScreen::cleanup()
{
	startInstructionBtnSignal.disconnect();
	startGameBtnSignal.disconnect();
	comeBackBtnSignal.disconnect();
	comeBackTimerStop();
}

void IntroScreen::mouseEvents()
{	
	MouseEvent event = _game->getMouseEvent();	

	if (!_game->freezeLocation && event.isLeftDown())
	{
		if(state == INIT)
		{
			nextState = SHOW_INVITE;			
			changeState();	
		}
		else comeBackTimerStart();
	}
}

void IntroScreen::keyEvents()
{
	#ifdef debug
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

void IntroScreen::update() 
{	
	#ifdef kinectInUse
		isPeopleInFrame = kinect().getSkeletsInFrame()!=0;
	#endif	

	if(_game->freezeLocation) return;

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
					debugString = "���� ���-�� � ����  ";//+to_string(kinect().getSkeletsInFrame());
				}
				else
				{
					debugString = "����� ��� � ����, \n����������� �� ������� ����� ���������� ����� : "+to_string(getSecondsToComeBack());
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
				debugString = "����������� �� ������� ����� ���������� ����� : "+to_string(getSecondsToComeBack());	
			#endif
		break;
	}	
}

void IntroScreen::draw() 
{
	gl::enableAlphaBlending();	
	gl::color( Color::white());
	Rectf centeredRect = Rectf( 0.0f, 0.0f, getWindowWidth(), getWindowHeight() ).getCenteredFit( getWindowBounds(),true );	

	switch (state)
	{
		case INIT:			
			drawInitElements();
		break;

		case SHOW_INVITE:
			gl::draw( playImage, centeredRect);	
			startInstructionBtn->draw();
			comeBackBtn->draw();
		break;

		case SHOW_INSTRUCTION:
			gl::draw( instructionImage, centeredRect);	
			startGameBtn->draw();
			comeBackBtn->draw();
		break;

		case START_GAME:			
		
		break;
	}

	#ifdef debug
		Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	#endif	

	if(_game->freezeLocation)
	{
		gl::color(ColorA(0.01, 0.31, 0.62, alphaAnimate));
		//gl::color(Color::hex(0x011f3e));
		gl::drawSolidRect(Rectf( 0.0f, 0.0f, getWindowWidth(), getWindowHeight()));
		gl::color(Color::white());
	}	
}

void IntroScreen::drawInitElements() 
{
	gl::pushMatrices();
		gl::translate(catAnimate, 410);
		gl::draw(cat);
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(150, logoAnimate);
		gl::draw(logo);
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(547, textAnimateY);
		gl::draw(text1);
	gl::popMatrices();
}

void IntroScreen::changeState() 
{
	_game->freezeLocation = true;
	timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &IntroScreen::animationFinished, this ) );	
}

void IntroScreen::animationFinished() 
{
	_game->freezeLocation = false;
	state = nextState;
	
	switch (state)
	{
		case INIT:				
			startInstructionBtnSignal.disconnect();
			comeBackBtnSignal.disconnect();
			startGameBtnSignal.disconnect();		
		break;

		case SHOW_INVITE:	
			if (!startInstructionBtnSignal.connected())
				startInstructionBtnSignal = startInstructionBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));

			if (!comeBackBtnSignal.connected())
				comeBackBtnSignal		  = comeBackBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::gotoFirstScreen, this));
				startGameBtnSignal.disconnect();
		break;

		case SHOW_INSTRUCTION:	
			if (!startGameBtnSignal.connected())
				startGameBtnSignal		  = startGameBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::startGameBtnDown, this));

			startInstructionBtnSignal.disconnect();
		break;

		case START_GAME:		
			_game->changeState(MainGameScreen::Instance());
		break;
	}	
}