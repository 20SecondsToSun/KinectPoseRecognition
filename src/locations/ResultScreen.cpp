#include "ResultScreen.h"

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

	popup().setup();
}

void ResultScreen::init( LocationEngine* game)
{
	_game = game;	

	isChangingStateNow = false;	
	canShowResultImages = false;
	isButtonsInit = false;
	goingOut	= false;	

	qrCode.reset();
	popup().reset();
	server().reset();	

	alphaFinAnimate = 0;
	
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
		photoLoadingSignal = photoMaker().photoLoadEvent.connect(boost::bind(&ResultScreen::photoLoaded, this));		
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStartFinished, this ) );		
	}
	else
	{
		state = SORRY_GO_HOME;	
		comeBackSignal = comeBackBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::gotoFirstScreen, this));
		comeBackTimerStart();
	}
}

void ResultScreen::animationStartFinished()
{
	state = PHOTO_LOADING_FROM_DIRECTORY;
}

void ResultScreen::photoLoaded()
{
	photoLoadingSignal.disconnect();
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
	photoSaveSignal =	photoMaker().photoSaveEvent.connect(boost::bind(&ResultScreen::photoSaved, this));
	isChangingStateNow = false;	
	photoMaker().resizeFinalImages();
	photoMaker().saveFinalImages();
}

void ResultScreen::photoSaved()
{
	photoSaveSignal.disconnect();
	isChangingStateNow = true;	
	timeline().apply( &alphaAnimate, 1.0f,  0.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationPhotoSavedFinished, this ) ).delay(0.3f);	
}

void ResultScreen::animationPhotoSavedFinished()
{
	canShowResultImages = true;	
	
	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		alphaAnimateComics[i] = 0;
		timeline().apply( &alphaAnimateComics[i], 1.0f, 0.7f, EaseOutCubic() ).delay(0.5f*i);		
	}
	
	if (Params::isNetConnected == false)
	{
		state = NET_OFF_LOCATION_READY;		
		isChangingStateNow = false;	
		initButtons();
		comeBackTimerStart();	
	}
	else
	{
		isChangingStateNow = true;	
		state = CHECKING_NET_CONNECTION;
		serverSignalConnectionCheck = server().serverCheckConnectionEvent.connect( 
			boost::bind(&ResultScreen::serverSignalConnectionCheckHandler, this) 
		);

		server().checkConnection();
	}
}

void ResultScreen::serverSignalConnectionCheckHandler()
{	
	serverSignalConnectionCheck.disconnect();

	if(server().isConnected == false)
	{
		isChangingStateNow = false;	
		state = NET_OFF_LOCATION_READY;		
		initButtons();
		comeBackTimerStart();	
	}
	else
	{
		isChangingStateNow = true;	
		state = PHOTO_LOADING_TO_SERVER;			
		timeline().apply( &alphaAnimate, 0.0f,  1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStart2ServerLoadFinished, this ) );		
	}
}

void ResultScreen::animationStart2ServerLoadFinished()
{	
	serverSignalLoadingCheck = server().serverLoadingPhotoEvent.connect( 
			boost::bind(&ResultScreen::serverLoadingPhotoHandler, this) 
		);
	server().sendPhoto(Params::getFinalImageStoragePath());
}

void ResultScreen::serverLoadingPhotoHandler()
{
	serverSignalLoadingCheck.disconnect();
	server().stopTimeout();
	isChangingStateNow = false;
	
	if (server().isPhotoLoaded)
	{
		state = LOADING_TO_SERVER_SUCCESS;		
		qrCode.setData(server().getBuffer(), server().getLink());		
	}
	else
	{
		state = LOADING_TO_SERVER_FAIL;			
	}

	initButtons();
	comeBackTimerStart();	
}

void ResultScreen::serverTimeoutHandler()
{
	serverSignalLoadingCheck.disconnect();	
	server().abortLoading();

	isChangingStateNow = false;		
	state = LOADING_TO_SERVER_FAIL;	

	initButtons();
	comeBackTimerStart();	
}

void ResultScreen::initButtons()
{
	if(server().isConnected)
	{
		fbSignal =facebookBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::facebookHandled, this));
		vkSignal =vkontakteBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::vkHandled, this));		
	}

	mailSignal =  mailBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::mailHandled, this));
	comeBackSignal = comeBackBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::gotoFirstScreen, this));

	isButtonsInit = true;
	console()<<"BUTTONS INIT ::::::::::::::::: "<<endl;	
}

void ResultScreen::initPopup(int type)
{
	state = POPUP_MODE;	
	popup().start(type);	
	disconnectButtons();
	closeSignal = popup().closeEvent.connect(boost::bind(&ResultScreen::closePopup, this));

	if (type == popupTypes::EMAIL)
	{
		sendToMailSignal = popup().sendEvent.connect(boost::bind(&ResultScreen::sendToMailHandler, this));
	}
}

void ResultScreen::sendToMailHandler()
{
	sendToMailSignal.disconnect();
	closeSignal.disconnect();
	//
	console()<<" BASE SAAAAAAAAAAAAAAVE  "<<server().isPhotoLoaded<<endl;		

	//
	initButtons();	
}


void ResultScreen::closePopup()
{	
	closeSignal.disconnect();
	sendToMailSignal.disconnect();
	initButtons();	
}

void ResultScreen::facebookHandled()
{	
	initPopup(popupTypes::FACEBOOK);
}

void ResultScreen::vkHandled()
{
	initPopup(popupTypes::VKONTAKTE);	
}

void ResultScreen::mailHandled()
{	
	initPopup(popupTypes::EMAIL);	

	//localPhotoSaveToBase();

	return;
	if (server().isConnected  )
	{
		serverSignalLoadingEmailCheck = server().sendToMailEvent.connect( 
			boost::bind(&ResultScreen::serverLoadingEmailHandler, this) 
		);
		
		server().sendToMail();
	}
	else
	{
		localPhotoSaveToBase();
	}
}

void ResultScreen::serverLoadingEmailHandler()
{
	serverSignalLoadingEmailCheck.disconnect();
}

void ResultScreen::localPhotoSaveToBase() 
{
	bool status = saver().saveImageIntoBase("yurikblech@gmail.com,up@mail.com", PlayerData::finalImageSurface);
}

void ResultScreen::gotoFirstScreen() 
{	
	if(!goingOut)
	{
		disconnectListeners();
		isChangingStateNow = true;
		goingOut = true;
		timeline().apply( &alphaFinAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationFinished, this ) );
	}
}

void ResultScreen::animationFinished() 
{
	_game->changeState(IntroScreen::Instance());
}

void ResultScreen::mouseEvents( )
{
	MouseEvent event = _game->getMouseEvent();	

	if(_game->isAnimationRunning()) return;		

	if (event.isLeftDown() && !returnTimer.isStopped())
	{	
		comeBackTimerStart();
	}
}

void ResultScreen::update() 
{	
	if (isComeBackTimerTouchFired() && isChangingStateNow == false)
	{		
		gotoFirstScreen();
		return;
	}

	switch (state)
	{
		case PHOTO_LOADING_FROM_DIRECTORY:
			photoMaker().loadFinalImages();			
		break;

		case PHOTO_LOADING_TO_SERVER:
			if (server().timerIsRunning() && server().getTimeoutSeconds()<= 0)
				serverTimeoutHandler();
		break;	
	}
}

void ResultScreen::draw() 
{
	gl::enableAlphaBlending();
	gl::clear(Color::black());	

	switch (state)
	{
		case PHOTO_LOADING_FROM_DIRECTORY:
		case INIT_STATE:			
			drawPhotoLoadingPreloader();
		break;

		case PHOTO_CREATE_COMICS:
			drawPhotoMakerPreloader();
		break;

		case PHOTO_LOADING_TO_SERVER:			
			drawServerPreloader();
		break;		

		case POPUP_MODE:
			drawPopup();			
			break;

		case SORRY_GO_HOME:
			drawUpsetScreen();
			comeBackBtn->draw();
		break;		

		default:
			break;
	}	

	if(popup().isDrawing == false)
	{
		drawResultImagesIfAllow();
		drawQRCodeIfAllow();	
		drawButtonsIfAllow();
	}

	drawFadeOutIfAllow();

	if (!returnTimer.isStopped())
	{
		#ifdef debug
			string debugString = "Возвращение на главный экран произойдет через : "+to_string(getSecondsToComeBack());	
			Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 940.0f), ColorA(1.f, 0.f, 0.f, 1.f));
		#endif
	}

	gl::disableAlphaBlending();	
		
	
	/*Utils::textFieldDraw("ФИНАЛЬНЫЙ ЭКРАН | УСПЕШНО: "+ to_string(PlayerData::score) +" из 3", 
		fonts().getFont("Helvetica Neue", 46), 
		Vec2f(400.f, 400.0f),
		ColorA(1.f, 1.f, 1.f, 1.f));*/	
}

void ResultScreen::drawPopup() 
{
	popup().draw();
}

void ResultScreen::drawResultImagesIfAllow() 
{
	if(canShowResultImages)
	{
		for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
		{
			if(PlayerData::playerData[i].isSuccess == false) continue;

			gl::pushMatrices();
				gl::translate(505*i, 200 );
				gl::scale(0.5f, 0.5f);	
				gl::color(ColorA(1,1,1,alphaAnimateComics[i]));
				gl::draw( PlayerData::playerData[i].imageTexture);
			gl::popMatrices();
		}
		gl::color(ColorA(1,1,1,1));
	}
}

void ResultScreen::drawQRCodeIfAllow() 
{
	if (server().isPhotoLoaded)	
		qrCode.draw();	
}

void ResultScreen::drawButtonsIfAllow() 
{
	if(isButtonsInit)
	{
		if(server().isConnected)
		{
			facebookBtn->draw();
			vkontakteBtn->draw();
		}

		mailBtn->draw();	
		comeBackBtn->draw();		
	}
}

void ResultScreen::drawFadeOutIfAllow() 
{
	if (goingOut)
	{
		gl::color(ColorA(0, 0, 0, alphaFinAnimate));	
		gl::drawSolidRect(getWindowBounds());
		gl::color(ColorA(0,0,0,1));
	}
}

void ResultScreen::drawPhotoLoadingPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Выгружаю фотографии...",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawPhotoMakerPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Подготавливаю комиксы...",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawServerPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));	
	Utils::textFieldDraw("Ожидаю сервер... " + to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawUpsetScreen() 
{
	//gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Вы не угадали ни одной позы...(((",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	//gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::disconnectListeners()
{
	serverSignalLoadingCheck.disconnect();
	serverSignalConnectionCheck.disconnect();	

	photoLoadingSignal.disconnect();
	photoSaveSignal.disconnect();

	closeSignal.disconnect();
	sendToMailSignal.disconnect();

	comeBackTimerStop();
	disconnectButtons();	

	server().abortLoading();	

	ph::clearTexture();

	console()<<"FINISHED!!!!!!!!!!!!!!!"<<endl;
}

void ResultScreen::disconnectButtons()
{
	comeBackSignal.disconnect();
	fbSignal.disconnect();
	vkSignal.disconnect();
	mailSignal.disconnect();
}

void ResultScreen::shutdown()
{
	disconnectListeners();
}

void ResultScreen::cleanup()
{
	disconnectListeners();
}

void ResultScreen::pause()
{
	
}

void ResultScreen::resume()
{
	
}
void ResultScreen::keyEvents()
{

}

void ResultScreen::handleEvents(  )
{

}