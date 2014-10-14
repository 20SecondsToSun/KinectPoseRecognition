#include "ResultScreen.h"
#include "PhotoMaker.h"
#include "TextureManager.h"
#include "Saver.h"

using namespace ci;
using namespace ci::app;
using namespace ReadyScreenDefaults;

ResultScreen ResultScreen::ResultScreenState;

void ResultScreen::setup()
{
	mailBtn = new ButtonColor(getWindow(),Rectf(100,700,500, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"E-mail");

	facebookBtn = new ButtonColor(getWindow(),Rectf(700,700,1100, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"Facebook");

	vkontakteBtn = new ButtonColor(getWindow(),Rectf(1300,700,1700, 800), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"Vkontakte");


	comeBackBtn = new ButtonColor(getWindow(), Rectf(1200,300, 1600, 400), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 46),
							"НАЗАД");

	photoMaker().photoLoadEvent.connect(boost::bind(&ResultScreen::photoLoaded, this));
	photoMaker().photoSaveEvent.connect(boost::bind(&ResultScreen::photoSaved, this));
}

void ResultScreen::init( LocationEngine* game)
{
	_game = game;	

	isChangingStateNow = false;
	isControlsBlocked = true;
	canShowResultImages = false;
	isButtonsInit = false;

	
	PlayerData::playerData[0].pathHiRes = "IMG_0003.jpg";
	PlayerData::playerData[1].pathHiRes = "IMG_0001.jpg";
	PlayerData::playerData[2].pathHiRes = "IMG_0002.jpg";

	for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		PlayerData::playerData[i].isSuccess = true;	
		PlayerData::playerData[i].storyCode = i;	
	}

	PlayerData::score = 3;

	if(PlayerData::score != 0)
	{
		state = INIT_STATE;
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStartFinished, this ) );		
	}
	else
		state = SORRY_GO_HOME;	

}

void ResultScreen::animationStartFinished()
{
	state = PHOTO_LOADING_FROM_DIRECTORY;
}

void ResultScreen::photoLoaded()
{
	isChangingStateNow = true;
	timeline().apply( &alphaAnimate, 1.0f, 0.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationPhotoLoadedFinished, this ) );
}

void ResultScreen::animationPhotoLoadedFinished()
{
	state = PHOTO_CREATE_COMICS;
	timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStart2Finished, this ) );	
}

void ResultScreen::animationStart2Finished()
{	
	isChangingStateNow = false;	
	photoMaker().resizeFinalImages();
	photoMaker().saveFinalImages();
}

void ResultScreen::photoSaved()
{
	isChangingStateNow = true;	
	timeline().apply( &alphaAnimate, 1.0f,  0.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationPhotoSavedFinished, this ) ).delay(0.3);
}

void ResultScreen::animationPhotoSavedFinished()
{
	canShowResultImages = true;

	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		timeline().apply( &alphaAnimateComics[i], 0.0f,  1.0f, 0.7f, EaseOutCubic() ).delay(0.5*i);		
	}
	
	if (Params::isNetConnected == false)
	{
		state = NET_OFF_LOCATION_READY;
		isNetConnection = false;	
		initButtons();
	}
	else
	{
		state = CHECKING_NET_CONNECTION;
		serverSignalConnectionCheck = server.serverCheckConnectionEvent.connect( 
			boost::bind(&ResultScreen::serverSignalConnectionCheckHandler, this) 
		);

		server.checkConnection();
	}
}

void ResultScreen::serverSignalConnectionCheckHandler()
{
	isNetConnection = server.isConnected;

	if(isNetConnection)
	{
		state = NET_OFF_LOCATION_READY;		
		initButtons();
	}
	else
	{		
		state = PHOTO_LOADING_TO_SERVER;	
		serverSignalLoadingCheck = server.serverLoadingPhotoEvent.connect( 
			boost::bind(&ResultScreen::serverLoadingPhotoHandler, this) 
		);
		server.sendPhoto(Params::getFinalImageStoragePath());
	}
}

void ResultScreen::serverLoadingPhotoHandler()
{	
	if (server.isLoading)
	{
		state = LOADING_TO_SERVER_SUCCESS;
		isServer = true;
		
		//qrCode.setTextureString(server.getBuffer());
		//qrCode.setLink(server.getLink());
		//qrCode.isReady = true;
		//qrCode.isError = false;	
	}
	else
	{
		state = LOADING_TO_SERVER_FAIL;
		isServer = false;
		//qrCode.isError = true;
	}

//	isServerFinishHisWork = true;

	initButtons();
}

void ResultScreen::initButtons()
{
	if(isNetConnection )
	{
		facebookBtn->addEventListener(MouseEvents::MOUSE_DOWN);
		vkontakteBtn->addEventListener(MouseEvents::MOUSE_DOWN);
		
		facebookBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::facebookHandled, this));
		vkontakteBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::vkHandled, this));		
	}


	mailBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::mailHandled, this));
	mailBtn->addEventListener(MouseEvents::MOUSE_DOWN);
		
	comeBackBtn->addEventListener(MouseEvents::MOUSE_DOWN);
	comeBackBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::gotoFirstScreen, this));
	comeBackTimerStart();

	isButtonsInit = true;
	console()<<"buttons init"<<endl;
}

void ResultScreen::facebookHandled()
{

}

void ResultScreen::vkHandled()
{

}

void ResultScreen::mailHandled()
{
	console()<<" isNetConnection!!!!!!!!:::::::: "<<isNetConnection<<"   "<<isServer<<endl;
	localPhotoSaveToBase();
	return;
	if (isNetConnection && isServer)
	{
		serverSignalLoadingEmailCheck = server.serverLoadingEmailEvent.connect( 
			boost::bind(&ResultScreen::serverLoadingEmailHandler, this) 
		);
		
		server.sendToMail();
	}
	else
	{
		localPhotoSaveToBase();
	}
}

void ResultScreen::serverLoadingEmailHandler()
{
	serverSignalLoadingEmailCheck.disconnect();

	console()<<" is email sent!!!!!!!!:::::::: "<<server.isEmailSent<<endl;
}



void ResultScreen::localPhotoSaveToBase() 
{
	bool status = saver().saveImageIntoBase("yurikblech@gmail.com,u@mail.com", PlayerData::finalImageSurface);

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



void ResultScreen::shutdown()
{
	serverSignalLoadingCheck.disconnect();
	serverSignalConnectionCheck.disconnect();
}

void ResultScreen::cleanup()
{
	serverSignalLoadingCheck.disconnect();
	serverSignalConnectionCheck.disconnect();


	comeBackTimerStop();
	comeBackBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	mailBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	facebookBtn->removeConnect(MouseEvents::MOUSE_DOWN);
	vkontakteBtn->removeConnect(MouseEvents::MOUSE_DOWN);

	ph::clearTexture();
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
		return;
	}

	switch (state)
	{
		case PHOTO_LOADING_FROM_DIRECTORY:
			photoMaker().loadFinalImages();
			//console()<<"   --------------  "<<photoMaker().isReady<<endl;
		break;

	}



}

void ResultScreen::draw() 
{
	gl::clear(Color::black());
	gl::enableAlphaBlending();

	switch (state)
	{
		case PHOTO_LOADING_FROM_DIRECTORY:
		case INIT_STATE:			
			drawPhotoLoadingPreloader();
		break;

		case PHOTO_CREATE_COMICS:
			drawPhotoMakerPreloader();
		break;

		case CHECKING_NET_CONNECTION:
			
			
			//drawPhotoMakerPreloader();
		break;

		case SERVER_EMAIL_ERROR:
			//drawPhotoMakerPreloader();
		break;

		case SERVER_INTERNET_ERROR:
			//drawPhotoMakerPreloader();
		break;

		case SORRY_GO_HOME:
			//drawPhotoMakerPreloader();
		break;

		

	default:
		break;
	}

	if(canShowResultImages)
	{
		for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
		{
			if(PlayerData::playerData[i].isSuccess == false) continue;

			gl::pushMatrices();
				gl::translate(505*i, 200 );
				gl::scale(0.5, 0.5);	
				gl::color(ColorA(1,1,1,alphaAnimateComics[i]));
				gl::draw( PlayerData::playerData[i].imageTexture);
			gl::popMatrices();
		}
	}

	if(isButtonsInit)
	{
		if(isNetConnection)
		{
			facebookBtn->draw();
			vkontakteBtn->draw();
		}

		mailBtn->draw();	
		comeBackBtn->draw();
	}

	return;


	int startX = 0;

	//for (int i = 0; i < 3; i++)
	//{
	//	
	//	if (PlayerData::isSuccess[i])
	//	{
	//		float scale = oneWidth/PlayerData::screenshot[i].getWidth();
	//		gl::pushMatrices();
	//		gl::translate(startX, 50);
	//		gl::scale(scale,scale);
	//		gl::draw(PlayerData::screenshot[i]);
	//		gl::popMatrices();

	//		startX += oneWidth;
	//	}
	//}
	
	Utils::textFieldDraw("ФИНАЛЬНЫЙ ЭКРАН | УСПЕШНО: "+ to_string(PlayerData::score) +" из 3", 
		fonts().getFont("Helvetica Neue", 46), 
		Vec2f(400.f, 400.0f),
		ColorA(1.f, 1.f, 1.f, 1.f));

	
	

	#ifdef debug
		string debugString = "Возвращение на главный экран произойдет через : "+to_string(getSecondsToComeBack());	
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

void ResultScreen::drawPhotoLoadingPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Загружаю фотографии...",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawPhotoMakerPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Подготавливаю комиксы...",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::animationFlashFinish() 
{
	//state = SCREESHOT_ANIM;		

	//timeline().apply( &startPhotoXY,Vec2f(0.0f, 0.0f), Vec2f(330.f, 215.f), 0.6f, EaseOutQuad() ).finishFn( bind( &ReadyScreen::animationLastFinish, this )  );	
	//timeline().apply( &startPhotoScale, Vec2f(1.f, 1.f),Vec2f(1130.0f/1920.0f, 1130.0f/1920.0f), 0.6f, EaseOutQuad() );
	
}

void ResultScreen::animationLastFinish() 
{

}

void ResultScreen::animationOutFinish()
{
	//_game->ChangeState(gotoLocation);
}