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

	lapaTv			  = *AssetManager::getInstance()->getTexture( "images/diz/lapa.png" );
	text1			  = *AssetManager::getInstance()->getTexture( "images/diz/nextText.png" );	
	discl			  = *AssetManager::getInstance()->getTexture( "images/diz/predup.png" );	

	bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );	

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
	comeBackBtn->setScreenField(Vec2f(0.0f, 958.0f));
	comeBackBtn->setDownState(comeBackBtnTex);

	bubbleAnimator().setup();
	catAnimator().setup();	
}

void IntroScreen::init( LocationEngine* game)
{	
	_game = game;	
	state = INIT;	

	_game->freezeLocation = false;
	isPeopleInFrame = false;	

	kinect().sleep(KINECT_SLEEP_SECONDS);
	initAnimateParam();

	deviceError = false;

	drawHandler = &IntroScreen::drawInitElements;
}

void IntroScreen::initAnimateParam()
{
	catAnimator().init();
	logoAnimate =  -1180.0f;
	timeline().apply( &logoAnimate, -1180.0f, 0.0f,  0.9f, EaseInOutQuart() );	
}

void IntroScreen::inviteAnimateParam() 
{
	bubbleAnimator().init();
	timeline().apply( &cat2AnimateVec, Vec2f(432.0f, 1080.0f), Vec2f(432.0f, 127.0f), 0.9f, EaseInOutQuart() );
	timeline().apply( &instructBtnAnimateVec, Vec2f(0.0f, 1080.0f), Vec2f(0.0f, 0.0f), 0.9f, EaseInOutQuart() );
}

void IntroScreen::initInstructionParam()
{	
	timeline().apply( &lapaTvAnimateVec, Vec2f(123.0f, 1080.0f), Vec2f(123.0f, 209.0f), 0.9f, EaseInOutQuart() );
	timeline().apply( &startBtnAnimateVec, Vec2f(0.0f, 1080.0f), Vec2f(0.0f, 0.0f), 0.9f, EaseInOutQuart() );

	timeline().apply( &textAnimateVec, Vec2f(995.0f, 72.0f), Vec2f(995.0f, 178.0f),  0.9f, EaseInOutQuart() );
	timeline().apply( &textAnimateAlpha, 0.0f, 1.0f,  0.9f, EaseInOutQuart() );
}

void IntroScreen::startInstructionBtnDown()
{	
	if(!_game->freezeLocation && !deviceError)
	{
		initInstructionParam();
		nextState = SHOW_INSTRUCTION;		
		changeState();
	}	
}

void IntroScreen::startGameBtnDown()
{
	if(!_game->freezeLocation && !deviceError)
	{
		nextState = START_GAME;			
		changeState();
	}
}

void IntroScreen::gotoFirstScreen() 
{
	if (state!= INIT && !_game->freezeLocation)
	{
		_game->freezeLocation = true;
		nextState = INIT;
		kinect().sleep(2);
		initAnimateParam();
		changeState();
		comeBackTimerStop();
	}
}

void IntroScreen::gotoInviteScreen() 
{
	if (state!= SHOW_INVITE && !_game->freezeLocation)
	{
		_game->freezeLocation = true;
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

	if (!_game->freezeLocation && event.isLeft() && type == MouseEvents::MOUSE_UP && !deviceError)
	{
		if(state == INIT)
			gotoInviteScreen();
		else 
			comeBackTimerStart();
	}
}

void IntroScreen::keyEvents()
{
	KeyEvent _event = _game->getKeyEvent();
}

void IntroScreen::update() 
{
	kinect().update();

	deviceError = !kinect().isConnected();

	if (deviceError) return;

#ifdef kinectUsed
	isPeopleInFrame = kinect().getSkeletsInFrame()!=0;
#endif

	if(_game->freezeLocation) return;

	switch (state)
	{
	case INIT:
		if (isPeopleInFrame)
			gotoInviteScreen();

		catAnimator().update();

#ifdef drawTimer
		debugString = "";
#endif
		break;

	case SHOW_INVITE:			
		if (isComeBackTimerKinectFired())
			gotoFirstScreen();

		bubbleAnimator().update();

#ifdef drawTimer
		debugString = to_string(getSecondsToComeBack());	
#endif
		break;

	case SHOW_INSTRUCTION:			
		if (isComeBackTimerTouchFired())
			gotoFirstScreen();

#ifdef drawTimer
		debugString = to_string(getSecondsToComeBack());	
#endif
		break;
	}	
}

void IntroScreen::draw() 
{
	gl::enableAlphaBlending();	
	gl::color( Color::white());

	if (deviceError)
	{
		Texture errorTexure = Utils::getTextField("ÊÈÍÅÊÒ ÂÛÊËÞ×ÅÍ", fonts().getFont("MaestroC", 114),  Color::white());	
		gl::draw(errorTexure, Vec2f(0.5f*(1920.0f - errorTexure.getWidth()), 348.0f));
		return;
	}

	(this->*drawHandler)();

#ifdef drawTimer
	Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.0f, 40.0f), RED);
#endif	

	if(_game->freezeLocation)
	{
		gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
		gl::draw(bg);
		gl::color(Color::white());
	}
}

void IntroScreen::drawInitElements()
{	
	gl::draw(logo, Vec2f(150.0f, logoAnimate));
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

	gl::draw(discl, Vec2f(0.0f, 1080.0f - discl.getHeight()));	
}

void IntroScreen::drawIstructionElements() 
{
	gl::draw(lapaTv, lapaTvAnimateVec);	

	gl::pushMatrices();
	gl::translate(startBtnAnimateVec);
	startGameBtn->draw();
	gl::popMatrices();

	gl::color(ColorA(1.0f, 1.0f, 1.0f, textAnimateAlpha));
	gl::draw(text1, textAnimateVec);

	gl::color(Color::white());
	comeBackBtn->draw();
}

void IntroScreen::changeState() 
{
	_game->freezeLocation = true;
	timeline().apply(&paramBad,  0.0f, 1.0f, 0.55f, EaseOutQuart() ).finishFn( [ & ]( )
	{
		_game->freezeLocation = false;
	});	

	animationFinished();
}

void IntroScreen::animationFinished() 
{
	switch (nextState)
	{
	case INIT:
		startInstructionBtnSignal.disconnect();
		comeBackBtnSignal.disconnect();
		startGameBtnSignal.disconnect();	
		bubbleAnimator().kill();

		drawHandler = &IntroScreen::drawInitElements;

		Utils::printVideoMemoryInfo();
		break;

	case SHOW_INVITE:	
		if (!startInstructionBtnSignal.connected())
			startInstructionBtnSignal = startInstructionBtn->mouseUpEvent.
			connect(boost::bind(&IntroScreen::startInstructionBtnDown, this));

		startGameBtnSignal.disconnect();
		comeBackBtnSignal.disconnect();

		catAnimator().kill();
		kinect().sleepKill();
		drawHandler = &IntroScreen::drawInviteElements;

		break;

	case SHOW_INSTRUCTION:	
		if (!startGameBtnSignal.connected())
			startGameBtnSignal = startGameBtn->mouseUpEvent.
			connect(boost::bind(&IntroScreen::startGameBtnDown, this));

		if (!comeBackBtnSignal.connected())
			comeBackBtnSignal  = comeBackBtn->mouseUpEvent.
			connect(boost::bind(&IntroScreen::gotoFirstScreen, this));

		bubbleAnimator().kill();
		startInstructionBtnSignal.disconnect();
		drawHandler = &IntroScreen::drawIstructionElements;
		break;

	case START_GAME:
		_game->freezeLocation = false;
		_game->changeState(MainGameScreen::Instance());
		break;
	}

	state = nextState;
}