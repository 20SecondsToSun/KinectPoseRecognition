#include "ResultScreen.h"

using namespace ci;
using namespace ci::app;
using namespace ReadyScreenDefaults;

ResultScreen ResultScreen::ResultScreenState;

void ResultScreen::setup()
{
	mailBtn = new ButtonColor(Rectf(100,700,500, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"E-mail");

	facebookBtn = new ButtonColor(Rectf(700,700,1100, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"Facebook");

	vkontakteBtn = new ButtonColor(Rectf(1300,700,1700, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"Vkontakte");


	createComeBackButton();

}

void ResultScreen::init( LocationEngine* game)
{
	_game = game;

	isChangingStateNow = false;

	oneWidth  = getWindowWidth() / 3;
	comeBackBtn->addEventListener(MouseEvents::MOUSE_DOWN);
	comeBackTimerStart();	
}

void ResultScreen::shutdown()
{
	//serverSignalCon.disconnect();
}

void ResultScreen::cleanup()
{
	//serverSignalCon.disconnect();
	comeBackTimerStop();
	comeBackBtn->removeConnect(MouseEvents::MOUSE_DOWN);
}

void ResultScreen::pause()
{
	
}
void ResultScreen::resume()
{
	
}

void ResultScreen::mouseEvents( )
{
	MouseEvent event = _game->getMouseEvent();	

	if(_game->isAnimationRunning()) return;		

	if (event.isLeftDown())
	{	
		comeBackTimerStart();
	}
}

void ResultScreen::keyEvents()
{

}

void ResultScreen::handleEvents(  )
{
}

void ResultScreen::update() 
{
	if (isChangingStateNow) return;

	if (isComeBackTimerTouchFired())
	{
		gotoFirstScreen();
	}
}

void ResultScreen::draw() 
{
	gl::clear(Color::black());
	gl::enableAlphaBlending();

	int startX = 0;

	for (int i = 0; i < 3; i++)
	{
		
		if (PlayerData::isSuccess[i])
		{
			float scale = oneWidth/PlayerData::screenshot[i].getWidth();
			gl::pushMatrices();
			gl::translate(startX, 50);
			gl::scale(scale,scale);
			gl::draw(PlayerData::screenshot[i]);
			gl::popMatrices();

			startX += oneWidth;
		}
	}
	
	Utils::textFieldDraw("ÔÈÍÀËÜÍÛÉ ÝÊÐÀÍ | ÓÑÏÅØÍÎ: "+ to_string(PlayerData::score) +" èç 3", 
		fonts().getFont("Helvetica Neue", 46), 
		Vec2f(400.f, 400.0f),
		ColorA(1.f, 1.f, 1.f, 1.f));

	mailBtn->draw();
	facebookBtn->draw();
	vkontakteBtn->draw();

	drawComeBackButton();

	#ifdef debug
		string debugString = "Âîçâðàùåíèå íà ãëàâíûé ýêðàí ïðîèçîéäåò ÷åðåç : "+to_string(getSecondsToComeBack());	
		Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	#endif

	
	if(isChangingStateNow)
	{
		gl::color(ColorA(0, 0, 0, alphaAnimate));
		gl::drawSolidRect(Rectf(0,0, getWindowWidth(), getWindowHeight()));
		gl::color(Color::white());
	}

	gl::disableAlphaBlending();	
}

void ResultScreen::animationFlashFinish() 
{
	//state = SCREESHOT_ANIM;		

	//timeline().apply( &startPhotoXY,Vec2f(0.0f, 0.0f), Vec2f(330.f, 215.f), 0.6f, EaseOutQuad() ).finishFn( bind( &ReadyScreen::animationLastFinish, this )  );	
	//timeline().apply( &startPhotoScale, Vec2f(1.f, 1.f),Vec2f(1130.0f/1920.0f, 1130.0f/1920.0f), 0.6f, EaseOutQuad() );
	
}

void ResultScreen::animationLastFinish() 
{
	#ifndef DEBUG
		//server.sendPhoto();
	#endif
}
/*
void ResultScreen::serverSignal()
{
	if (server.isResponseOK())
	{
		qrCode.setTextureString(server.getBuffer());
		qrCode.setLink(server.getLink());
		qrCode.isReady = true;
		qrCode.isError = false;
	}
	else
	{
		qrCode.isError = true;
	}

	isServerFinishHisWork = true;	
}*/

void ResultScreen::animationOutFinish()
{
	//_game->ChangeState(gotoLocation);
}

void ResultScreen::gotoFirstScreen() 
{
	isChangingStateNow = true;
	timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationFinished, this ) );	
}

void ResultScreen::animationFinished() 
{
	isChangingStateNow = false;
	_game->changeState(IntroScreen::Instance());
}