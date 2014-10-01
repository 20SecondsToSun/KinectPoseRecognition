#include "ResultScreen.h"

using namespace ci;
using namespace ci::app;
using namespace ReadyScreenDefaults;



ResultScreen ResultScreen::ResultScreenState;

void ResultScreen::setup()
{
	hintFont = ci::Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 46 );
}

void ResultScreen::init( LocationEngine* game)
{
	
	_game = game;


	oneWidth  = getWindowWidth() / 3;//PlayerData::score;

	//timeline().apply( &alphaFlash, 1.0f, 0.0f, 0.7f, EaseOutExpo() ).delay(0.2f).finishFn( bind( &ReadyScreen::animationFlashFinish, this )  );	
}

void ResultScreen::shutdown()
{
	//serverSignalCon.disconnect();
}

void ResultScreen::cleanup()
{
	//serverSignalCon.disconnect();
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

	#ifdef DEBUG

	if (event.isLeftDown())
	{
		if (backToStart.isDown(event.getPos()))
		{
			game->ChangeState(ScanFace::Instance());
		}
		else game->ChangeState(MainGame::Instance());
	}

	#endif
}

void ResultScreen::keyEvents()
{

}

void ResultScreen::handleEvents(  )
{
}

void ResultScreen::update() 
{
	
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

	
	Utils::textFieldDraw("ÔÈÍÀËÜÍÛÉ ÝÊÐÀÍ | ÓÑÏÅØÍÎ: "+ to_string(PlayerData::score) +" èç 3", &hintFont, Vec2f(400.f, 400.0f), ColorA(1.f, 1.f, 1.f, 1.f));

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