#include "IntroScreen.h"

using namespace ci;
using namespace ci::app;
using namespace colorsParams;

IntroScreen IntroScreen::IntroScreenState;

void IntroScreen::setup()
{		
	logo  = *AssetManager::getInstance()->getTexture( "images/diz/logo1.png" );	

	cat2   = *AssetManager::getInstance()->getTexture( "images/diz/cat2.png" );
	btnFon = *AssetManager::getInstance()->getTexture( "images/diz/btnFon.png" );
	paws   = *AssetManager::getInstance()->getTexture( "images/diz/paws.png" );

	lapaTv   = *AssetManager::getInstance()->getTexture( "images/diz/lapa.png" );
	text1 = *AssetManager::getInstance()->getTexture( "images/diz/nextText.png" );	
	
	bubbleAnimator().setup();
	catAnimator().setup();

	instructionImage  = *AssetManager::getInstance()->getTexture( "images/diz/instr.png" );

	Font *btnFont = fonts().getFont("Helvetica Neue", 46);
	
	/////////////////////////////////
	Texture startInstructionBtnTex   = *AssetManager::getInstance()->getTexture( "images/diz/btn_off.png" );
	Texture startInstructionBtnTexUp   = *AssetManager::getInstance()->getTexture( "images/diz/btn_on.png" );

	startInstructionBtn = new ButtonTex(startInstructionBtnTex,  "startInstruction");
	startInstructionBtn->setScreenField(Vec2f(502.0f, 505.0f));
	startInstructionBtn->setDownState(startInstructionBtnTexUp, Vec2f(0.0f, 5.0f));
	
	////////////////////////////////
	Texture startGameBtnTex   = *AssetManager::getInstance()->getTexture( "images/diz/nextBtn1.png" );
	Texture startGameBtnTexUp   = *AssetManager::getInstance()->getTexture( "images/diz/nextBtn2.png" );
	
	startGameBtn = new ButtonTex(startGameBtnTex,  "startGame");
	startGameBtn->setScreenField(Vec2f(953.0f, 626.0f));
	startGameBtn->setDownState(startGameBtnTexUp);
	
	///////////////////////////////
	
	Texture comeBackBtnTex   = *AssetManager::getInstance()->getTexture( "images/diz/toStart.png" );
		
	comeBackBtn = new ButtonTex(comeBackBtnTex,  "backtoStart");
	comeBackBtn->setScreenField(Vec2f(0.0f, 1080.0f - 169.0f));
	comeBackBtn->setDownState(comeBackBtnTex);
}

void IntroScreen::init( LocationEngine* game)
{	
	_game = game;
	drawHandler = &IntroScreen::drawInitElements;
	state = INIT;	

	_game->freezeLocation = false;
	isPeopleInFrame = false;	
	
	kinect().sleep(2);
	initAnimateParam();
}

void IntroScreen::initAnimateParam()
{
	catAnimator().init();	

	logoAnimate= -1180.0f;
	timeline().apply( &logoAnimate, 0.0f,  0.9f, EaseInOutQuart() );	
}

void IntroScreen::inviteAnimateParam() 
{
	bubbleAnimator().init();
	cat2AnimateVec = Vec2f(432.0f, 1080.0f);
	instructBtnAnimateVec = Vec2f(0.0f, 1080.0f);
	timeline().apply( &cat2AnimateVec, Vec2f(432.0f, 127.0f), 0.9f, EaseInOutQuart() );
	timeline().apply( &instructBtnAnimateVec, Vec2f(0.0f, 0.0f), 0.9f, EaseInOutQuart() );
}

void IntroScreen::initInstructionParam()
{
	lapaTvAnimateVec = Vec2f(123.0f, 1080.0f);
	startBtnAnimateVec = Vec2f(0.0f, 1080.0f);
	timeline().apply( &lapaTvAnimateVec, Vec2f(123.0f, 209.0f), 0.9f, EaseInOutQuart() );
	timeline().apply( &startBtnAnimateVec, Vec2f(0.0f, 0.0f), 0.9f, EaseInOutQuart() );

	textAnimateVec = Vec2f(995.0f, 72.0f);
	textAnimateAlpha = 0.0f;
	timeline().apply( &textAnimateVec, Vec2f(995.0f, 178.0f),  0.9f, EaseInOutQuart() );
	timeline().apply( &textAnimateAlpha, 1.0f,  0.9f, EaseInOutQuart() );
}

void IntroScreen::startInstructionBtnDown()
{	
	if(!_game->freezeLocation)
	{
		initInstructionParam();
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
		kinect().sleep(2);
		initAnimateParam();
		nextState = INIT;		
		changeState();		
		comeBackTimerStop();	
	}
}

void IntroScreen::gotoInviteScreen() 
{
	if (state!= SHOW_INVITE && !_game->freezeLocation)
	{
		inviteAnimateParam();
		nextState = SHOW_INVITE;
		changeState();
	}
}

void IntroScreen::cleanup()
{
	startInstructionBtnSignal.disconnect();
	startGameBtnSignal.disconnect();
	comeBackBtnSignal.disconnect();
	comeBackTimerStop();
}

void IntroScreen::mouseEvents(int type)
{	
	MouseEvent event = _game->getMouseEvent();	

	if (!_game->freezeLocation && event.isLeft() && type == MouseEvents::MOUSE_UP)
	{
		if(state == INIT)
			gotoInviteScreen();
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
	#ifdef kinectUsed
		isPeopleInFrame = kinect().getSkeletsInFrame()!=0;
	#endif		

	if(_game->freezeLocation) return;

	switch (state)
	{
		case INIT:
			if (isPeopleInFrame)
				gotoInviteScreen();
		
			#ifdef debug
				debugString = "";
			#endif
			catAnimator().update();
		break;

		case SHOW_INVITE:			
			if (isComeBackTimerKinectFired())			
				gotoFirstScreen();
			
			#ifdef debug
				debugString = to_string(getSecondsToComeBack());	
			#endif
			bubbleAnimator().update();
		break;

		case SHOW_INSTRUCTION:			
			if (isComeBackTimerTouchFired())
				gotoFirstScreen();
			#ifdef debug
				debugString = to_string(getSecondsToComeBack());	
			#endif
		break;
	}	
}

void IntroScreen::draw() 
{
	gl::enableAlphaBlending();	
	gl::color( Color::white());

	(this->*drawHandler)();

	#ifdef debug
		Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), RED);
	#endif	

	if(_game->freezeLocation)
	{
		gl::color(ColorA(BLUE_FADE.r, BLUE_FADE.g, BLUE_FADE.b, alphaAnimate));		
		gl::drawSolidRect(Rectf( 0.0f, 0.0f, getWindowWidth(), getWindowHeight()));
		gl::color(Color::white());
	}	
}

void IntroScreen::drawInitElements()
{	
	gl::draw(logo, Vec2f(150, logoAnimate));
	catAnimator().draw();
}

void IntroScreen::drawInviteElements() 
{
	bubbleAnimator().draw();

	gl::pushMatrices();
		gl::translate(cat2AnimateVec);
		gl::draw(cat2);
		gl::draw(btnFon, Vec2f(29.0f, 353.0f));	
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(instructBtnAnimateVec);
		startInstructionBtn->draw();
		gl::draw(paws, Vec2f(435.0f, 429.0f));
	gl::popMatrices();
}

void IntroScreen::drawIstructionElements() 
{
	gl::pushMatrices();
		gl::translate(lapaTvAnimateVec);
		gl::draw(lapaTv);
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(startBtnAnimateVec);
		startGameBtn->draw();
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(textAnimateVec);
		gl::color(ColorA(1.0f, 1.0f, 1.0f, textAnimateAlpha));
		gl::draw(text1);
	gl::popMatrices();
	gl::color(Color::white());
	
	comeBackBtn->draw();
}

void IntroScreen::changeState() 
{
	_game->freezeLocation = true;
	timeline().apply( &alphaAnimate,  0.0f, 1.0f, 0.4f, EaseOutCubic() ).finishFn( bind( &IntroScreen::animationFinished, this ) );//.delay(0.5);	
}

void IntroScreen::animationFinished() 
{
	_game->freezeLocation = false;

	switch (nextState)
	{
		case INIT:				
			startInstructionBtnSignal.disconnect();
			comeBackBtnSignal.disconnect();
			startGameBtnSignal.disconnect();	
			bubbleAnimator().kill();
			
			drawHandler = &IntroScreen::drawInitElements;
		break;

		case SHOW_INVITE:	
			if (!startInstructionBtnSignal.connected())
				startInstructionBtnSignal = startInstructionBtn->mouseUpEvent.connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));		

			startGameBtnSignal.disconnect();
			comeBackBtnSignal.disconnect();

			catAnimator().kill();
			kinect().sleepKill();
			drawHandler = &IntroScreen::drawInviteElements;
			
		break;

		case SHOW_INSTRUCTION:	
			if (!startGameBtnSignal.connected())
				startGameBtnSignal		  = startGameBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::startGameBtnDown, this));

			if (!comeBackBtnSignal.connected())
				comeBackBtnSignal  = comeBackBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::gotoFirstScreen, this));

			bubbleAnimator().kill();			
			startInstructionBtnSignal.disconnect();			
			drawHandler = &IntroScreen::drawIstructionElements;
		break;

		case START_GAME:
			_game->changeState(MainGameScreen::Instance());
		break;
	}

	state = nextState;
}