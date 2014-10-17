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
	emailPopup().setup();
}

void ResultScreen::init( LocationEngine* game)
{
	_game = game;	

	_game->freezeLocation = false;
	canShowResultImages = false;
	isButtonsInit = false;
	isLeaveAnimation	= false;	

	qrCode.reset();
	popup().reset();
	server().reset();	

	alphaFinAnimate = 0;

	#ifdef debug
		PlayerData::playerData[0].pathHiRes = "IMG_0003.jpg";
		PlayerData::playerData[1].pathHiRes = "IMG_0001.jpg";
		PlayerData::playerData[2].pathHiRes = "IMG_0002.jpg";

		if (Params::photoFromDirError)
			PlayerData::playerData[0].pathHiRes = "IMG_00.jpg";

		for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
		{
			PlayerData::playerData[i].isSuccess = false;	
			PlayerData::playerData[i].storyCode = 0;	
		}

		for (size_t  i = 0; i < PlayerData::score; i++)
		{
			PlayerData::playerData[i].isSuccess = true;	
			PlayerData::playerData[i].storyCode = i;	
		}
	#endif	

	if(PlayerData::score != 0)
	{
		state = INIT_STATE;
		photoLoadingFromDirSignal = photoMaker().photoLoadEvent.connect(boost::bind(&ResultScreen::photoLoadedFromDirHandler, this));	
		photoLoadingFromDirErrorSignal = photoMaker().photoLoadErrorEvent.connect(boost::bind(&ResultScreen::photoLoadeFromDirErrorHandler, this));	
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStartFinished, this ) );		
	}
	else
	{
		state = SORRY_GO_HOME;	
		comeBackSignal = comeBackBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
		comeBackTimerStart();
	}
}

void ResultScreen::animationStartFinished()
{
	state = PHOTO_LOADING_FROM_DIRECTORY;	
}

void ResultScreen::photoLoadedFromDirHandler()
{	
	_game->freezeLocation = true;

	photoLoadingFromDirSignal.disconnect();
	photoLoadingFromDirErrorSignal.disconnect();
	photoMaker().resizeFinalImages();
	timeline().apply( &alphaAnimate, 0.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationPhotoSavedFinished, this ) ).delay(0.3f);
}

void ResultScreen::photoLoadeFromDirErrorHandler()
{
	state = ERROR_STATE;	
	photoLoadingFromDirSignal.disconnect();
	photoLoadingFromDirErrorSignal.disconnect();
	comeBackSignal = comeBackBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
	comeBackTimerStart();
}

void ResultScreen::animationPhotoSavedFinished()
{
	canShowResultImages = true;	

	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			alphaAnimateComics[i] = 0;
			timeline().apply( &alphaAnimateComics[i], 1.0f, 0.7f, EaseOutCubic() ).delay(0.5f*i);		
		}
	}
	
	if (Params::isNetConnected == false)
	{
		state = NET_OFF_LOCATION_READY;		
		_game->freezeLocation = false;	
		connectButtons();
		comeBackTimerStart();	
	}
	else
	{	
		_game->freezeLocation = true;	
		state = CHECKING_NET_CONNECTION;	
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowChekConnection, this ) );		
	}
}

void ResultScreen::animationShowChekConnection()
{
	serverSignalConnectionCheck = server().serverCheckConnectionEvent.connect(
		boost::bind(&ResultScreen::serverSignalConnectionCheckHandler, this)
	);
	server().checkConnection();
}

void ResultScreen::serverSignalConnectionCheckHandler()
{
	server().stopTimeout();
	serverSignalConnectionCheck.disconnect();
	timeline().apply( &alphaAnimate, 0.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationHideChekConnection, this ) );
}

void ResultScreen::animationHideChekConnection()
{
	if(server().isConnected == false)
	{
		_game->freezeLocation = false;	
		state = NET_OFF_LOCATION_READY;	
		connectButtons();
		comeBackTimerStart();	
	}
	else
	{
		_game->freezeLocation = true;	
		state = PHOTO_LOADING_TO_SERVER;	
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowServerPhotoLoad, this ) );	
	}
}

void ResultScreen::animationShowServerPhotoLoad()
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
	timeline().apply( &alphaAnimate, 0.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationHideServerPhotoLoad, this ) );	
}

void ResultScreen::animationHideServerPhotoLoad()
{
	_game->freezeLocation = false;
	
	if (server().isPhotoLoaded)
	{
		state = LOADING_TO_SERVER_SUCCESS;		
		qrCode.setData(server().getBuffer(), server().getLink());		
	}
	else
	{
		state = LOADING_TO_SERVER_FAIL;			
	}

	connectButtons();
	comeBackTimerStart();	
}

void ResultScreen::serverTimeoutHandler()
{
	serverSignalLoadingCheck.disconnect();	
	server().abortLoading();

	_game->freezeLocation = false;		
	state = LOADING_TO_SERVER_FAIL;	

	connectButtons();
	comeBackTimerStart();	
}

void ResultScreen::connectButtons()
{
	if(server().isConnected)
	{
		fbSignal =facebookBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::facebookBtnHandler, this));
		vkSignal =vkontakteBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::vkBtnHandler, this));		
	}

	mailSignal =  mailBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::openEmailBtnHandler, this));
	comeBackSignal = comeBackBtn->mouseDownEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));

	isButtonsInit = true;
	console()<<"BUTTONS INIT ............. "<<endl;	
}

void ResultScreen::facebookBtnHandler()
{	
	initPopup(popupTypes::FACEBOOK);
}

void ResultScreen::vkBtnHandler()
{
	initPopup(popupTypes::VKONTAKTE);	
}

void ResultScreen::openEmailBtnHandler()
{	
	initPopup(popupTypes::EMAIL);	
}

void ResultScreen::initPopup(int type)
{
	if (type == popupTypes::EMAIL)
	{
		state = POPUP_EMAIL;	
		emailPopup().show();
		closeEmailPopupSignal = emailPopup().closeEvent.connect(boost::bind(&ResultScreen::closeEmailPopup, this));
		sendToMailSignal = emailPopup().sendEvent.connect(boost::bind(&ResultScreen::sendToEmailBtnHandler, this));
		disconnectButtons();
	}
	else if (type == popupTypes::VKONTAKTE || type == popupTypes::FACEBOOK)
	{
		state = POPUP_MODE;	
		popup().start(type);	
		disconnectButtons();
		closeSocialPopupSignal = popup().closeEvent.connect(boost::bind(&ResultScreen::closeSocialPopup, this));
	}
}

void ResultScreen::closeEmailPopup()
{
	console()<<"CLOSE EMAIL POPUP DONE"<<endl;
	state = DEFAULT_STATE;		
	closeEmailPopupSignal.disconnect();
	sendToMailSignal.disconnect();
	emailPopup().disconnectAll();
	connectButtons();	
}

void ResultScreen::sendToEmailBtnHandler()
{
	sendToMailSignal.disconnect();
	closeEmailPopupSignal.disconnect();
	emailPopup().disconnectAll();

	console()<<"TRY TO SAVE PHOTOS.......... "<< server().isPhotoLoaded <<endl;
	if (server().isPhotoLoaded)
	{
		sendPhotoToEmail();
	}
	else
	{
		savePhotoToLocalBase();
		connectButtons();	
	}
}

void ResultScreen::closeSocialPopup()
{
	state = DEFAULT_STATE;		
	closeSocialPopupSignal.disconnect();
	connectButtons();	
}

void ResultScreen::sendPhotoToEmail() 
{	
	state = PHOTO_SENDING_TO_MAIL;
	_game->freezeLocation = true;	
	timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowSendingToMailText, this ) );		
}

void ResultScreen::animationShowSendingToMailText() 
{	
	serverSignalLoadingEmailCheck = server().sendToMailEvent.connect( 
			boost::bind(&ResultScreen::serverLoadingEmailHandler, this) 
		);	

	server().sendToMail(emailPopup().getEmails());
}

void ResultScreen::serverLoadingEmailHandler()
{
	server().stopTimeout();
	_game->freezeLocation = false;	

	if (server().isEmailSent == false)
	{		
		savePhotoToLocalBase();
	}
	else
	{
		console()<<"SERVER SAVE MAILS!!!!! "<<endl;
	}

	timeline().apply( &alphaAnimate, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowSendingToMailTextOut, this ) );	
}

void ResultScreen::animationShowSendingToMailTextOut() 
{
	serverSignalLoadingEmailCheck.disconnect();
	connectButtons();
	comeBackTimerStart();
	state = LOADING_TO_SERVER_SUCCESS;	
}

void ResultScreen::savePhotoToLocalBase() 
{	
	state = SAVING_LOCALY_SUCCESS;
	console()<<"SERVER ERROR. SAVE LOCALY "<<emailPopup().getEmailsInString()<<endl;
	bool status = saver().saveImageIntoBase(emailPopup().getEmailsInString(), PlayerData::finalImageSurface);
}

void ResultScreen::closeScreenHandler() 
{	
	if(!isLeaveAnimation)
	{
		disconnectListeners();
		_game->freezeLocation = true;
		isLeaveAnimation = true;
		timeline().apply( &alphaFinAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationLeaveLocationFinished, this ) );
	}
}

void ResultScreen::animationLeaveLocationFinished() 
{
	_game->freezeLocation = false;
	_game->changeState(IntroScreen::Instance());
}

void ResultScreen::mouseEvents( )
{
	if(_game->freezeLocation) return;

	MouseEvent event = _game->getMouseEvent();

	if (event.isLeftDown() && !returnTimer.isStopped())
	{	
		comeBackTimerStart();
	}
}

void ResultScreen::update() 
{	
	if (isComeBackTimerTouchFired() && _game->freezeLocation == false)
	{		
		closeScreenHandler();
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

		case CHECKING_NET_CONNECTION:
			drawNetConnectionPreloader();
		break;		

		case PHOTO_LOADING_TO_SERVER:			
			drawServerPreloader();
		break;			

		case PHOTO_SENDING_TO_MAIL:			
			drawSendingToMailPreloader();
		break;			

		case POPUP_EMAIL:	
			emailPopup().draw();			
			break;

		case POPUP_MODE:	
			drawPopup();			
			break;
			

		case SORRY_GO_HOME:
			drawUpsetScreen();
			comeBackBtn->draw();
		break;		

		case ERROR_STATE:
			drawErrorScreen();
			comeBackBtn->draw();
		break;	

		default:
			break;
	}	

	if(state!= POPUP_EMAIL && state!=POPUP_MODE)
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
			if(PlayerData::playerData[i].isSuccess )
			{
				gl::pushMatrices();
					gl::translate(505*i, 200 );
					gl::scale(0.5f, 0.5f);	
					gl::color(ColorA(1,1,1,alphaAnimateComics[i]));					
					gl::draw( PlayerData::getDisplayingTexture(i));
				gl::popMatrices();
			}
			
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
	if (isLeaveAnimation)
	{
		gl::color(ColorA(0, 0, 0, alphaFinAnimate));	
		gl::drawSolidRect(getWindowBounds());
		gl::color(ColorA(0,0,0,1));
	}
}

void ResultScreen::drawPhotoLoadingPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Выгружаю фотографии... "+ to_string(photoMaker().getElapsedSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawNetConnectionPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Проверяю соединение... "+ to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawServerPreloader() 
{	
	gl::color(ColorA(1, 1, 1, alphaAnimate));	
	Utils::textFieldDraw("Ожидаю сервер... " + to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawSendingToMailPreloader() 
{	
	gl::color(ColorA(1, 1, 1, alphaAnimate));	
	Utils::textFieldDraw("Отправляю на почту... " + to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawUpsetScreen() 
{
	//gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Вы не угадали ни одной позы...(((",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	//gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::drawErrorScreen() 
{
	//gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Что-то пошло не так...(((",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	//gl::color(ColorA(1, 1, 1, 1));
}

void ResultScreen::disconnectListeners()
{
	photoMaker().stopTimer();
	server().stopTimeout();

	serverSignalConnectionCheck.disconnect();
	serverSignalLoadingCheck.disconnect();
	serverSignalLoadingEmailCheck.disconnect();
	server().abortLoading();	

	photoLoadingFromDirSignal.disconnect();	
	photoLoadingFromDirErrorSignal.disconnect();

	closeSocialPopupSignal.disconnect();
	closeEmailPopupSignal.disconnect();
	sendToMailSignal.disconnect();	
	emailPopup().disconnectAll();

	comeBackTimerStop();
	disconnectButtons();	

	ph::clearTexture();
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

void ResultScreen::handleEvents()
{

}