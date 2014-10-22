#include "IntroScreen.h"

using namespace ci;
using namespace ci::app;
using namespace colorsParams;

IntroScreen IntroScreen::IntroScreenState;

void IntroScreen::setup()
{	
	cat	  = *AssetManager::getInstance()->getTexture( "images/diz/cat.png" );
	logo  = *AssetManager::getInstance()->getTexture( "images/diz/logo1.png" );	
	text1 = *AssetManager::getInstance()->getTexture( "images/diz/text1.png" );	


	cat2   = *AssetManager::getInstance()->getTexture( "images/diz/cat2.png" );
	btnFon = *AssetManager::getInstance()->getTexture( "images/diz/btnFon.png" );
	paws   = *AssetManager::getInstance()->getTexture( "images/diz/paws.png" );
	
	bubbleAnimator().setup();

	instructionImage  = *AssetManager::getInstance()->getTexture( "images/diz/instr.png" );

	Font *btnFont = fonts().getFont("Helvetica Neue", 46);

	Texture startInstructionBtnTex   = *AssetManager::getInstance()->getTexture( "images/diz/btn_off.png" );
	Texture startInstructionBtnTexUp   = *AssetManager::getInstance()->getTexture( "images/diz/btn_on.png" );

	startInstructionBtn = new ButtonTex(startInstructionBtnTex,  "startInstruction");
	startInstructionBtn->setScreenField(Vec2f(502, 505));
	startInstructionBtn->setDownState(startInstructionBtnTexUp);

	startGameBtn		= new ButtonColor(Rectf(1200,700,1600, 800),  RED, btnFont, "ÍÀ×ÀÒÜ ÈÃÐÓ");
	comeBackBtn			= new ButtonColor(Rectf(1520,980, 1920, 1080), RED, btnFont, "ÍÀÇÀÄ");
}

void IntroScreen::init( LocationEngine* game)
{	
	_game = game;
	state = INIT;	

	_game->freezeLocation = false;
	isPeopleInFrame = false;	
	
	initAnimateParam();
}

void IntroScreen::initAnimateParam()
{
	catAnimate = -537;
	timeline().apply( &catAnimate, 0.0f, 0.9f, EaseInOutQuart() );

	logoAnimate= -1180;
	timeline().apply( &logoAnimate, 0.0f,  0.9f, EaseInOutQuart() );

	textAnimateY = 860;
	textAnimateAlpha = 0;
	timeline().apply( &textAnimateY, 885.0f,  0.9f, EaseInOutQuart() );
	timeline().apply( &textAnimateAlpha, 1.0f,  0.9f, EaseInOutQuart() );
}

void IntroScreen::inviteAnimateParam() 
{
	bubbleAnimator().init();
	cat2AnimateVec = Vec2f(432, 1080);
	instructBtnAnimateVec = Vec2f(0, 1080);
	timeline().apply( &cat2AnimateVec, Vec2f(432, 127), 0.9f, EaseInOutQuart() );
	timeline().apply( &instructBtnAnimateVec, Vec2f(0, 0), 0.9f, EaseInOutQuart() );
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
		nextState = SHOW_INVITE;
		inviteAnimateParam();
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
	#ifdef kinectInUse
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

	switch (state)
	{
		case INIT:			
			drawInitElements();
		break;

		case SHOW_INVITE:
			drawInviteElements();			
		break;

		case SHOW_INSTRUCTION:
			gl::draw( instructionImage);	
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
		gl::color(ColorA(38 /255.0,184/255.0,240/255.0, alphaAnimate));
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
		gl::color(ColorA(1,1,1,textAnimateAlpha));
		gl::draw(text1);
		gl::color(Color::white());
	gl::popMatrices();
}

void IntroScreen::drawInviteElements() 
{
	bubbleAnimator().draw();

	gl::pushMatrices();
		gl::translate(cat2AnimateVec);
		gl::draw(cat2);	
		gl::translate(Vec2f(29, 353));
		gl::draw(btnFon);	
	gl::popMatrices();

	gl::pushMatrices();
		gl::translate(instructBtnAnimateVec);
		startInstructionBtn->draw();

		gl::pushMatrices();
			gl::translate(Vec2f(435, 429));
			gl::draw(paws);	
		gl::popMatrices();
	gl::popMatrices();

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
	state = nextState;
	
	switch (state)
	{
		case INIT:				
			startInstructionBtnSignal.disconnect();
			comeBackBtnSignal.disconnect();
			startGameBtnSignal.disconnect();	
			bubbleAnimator() .kill();
		break;

		case SHOW_INVITE:	
			if (!startInstructionBtnSignal.connected())
				startInstructionBtnSignal = startInstructionBtn->mouseUpEvent.connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));

			if (!comeBackBtnSignal.connected())
				comeBackBtnSignal		  = comeBackBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::gotoFirstScreen, this));
				startGameBtnSignal.disconnect();
		break;

		case SHOW_INSTRUCTION:	
			if (!startGameBtnSignal.connected())
				startGameBtnSignal		  = startGameBtn->mouseDownEvent.connect(boost::bind(&IntroScreen::startGameBtnDown, this));

			bubbleAnimator() .kill();
			startInstructionBtnSignal.disconnect();
		break;

		case START_GAME:		
			//_game->changeState(MainGameScreen::Instance());
		break;
	}	
}