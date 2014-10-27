#pragma warning(push)
#pragma warning(disable: 4244)
#include "ResultScreen.h"

using namespace ci;
using namespace ci::app;
using namespace colorsParams;

ResultScreen ResultScreen::ResultScreenState;

void ResultScreen::setup()
{
	Font *btnFont = fonts().getFont("Helvetica Neue", 46);

	Texture emailBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/mailBtn.png");
	mailBtn = new ButtonTex(emailBtnTex,  "E-mail");
	mailBtn->setScreenField(Vec2f(674.0f, 672.0f));
	mailBtn->setDownState(emailBtnTex);

	Texture backBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/playMore.png");
	comeBackBtn = new ButtonTex(backBtnTex,  "BACK");
	comeBackBtn->setScreenField(Vec2f(1578.0f, 655.0f));
	comeBackBtn->setDownState(backBtnTex);

	Texture facebookBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/fb_off.png");
	Texture facebookBtnTexDown   = *AssetManager::getInstance()->getTexture("images/serverScreen/fb_on.png");	
	facebookBtn = new ButtonTex(facebookBtnTex,  "Facebook");
	facebookBtn->setScreenField(Vec2f(158.0f, 672.0f));
	facebookBtn->setDownState(facebookBtnTexDown);

	Texture vkontakteBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/vk_off.png");
	Texture vkontakteBtnTexDown   = *AssetManager::getInstance()->getTexture("images/serverScreen/vk_on.png");	
	vkontakteBtn = new ButtonTex(vkontakteBtnTex,  "Vkontakte");
	vkontakteBtn->setScreenField(Vec2f(158.0f, 840.0f));
	vkontakteBtn->setDownState(vkontakteBtnTexDown);

	postPhotoTextTex       = *AssetManager::getInstance()->getTexture("images/serverScreen/postPhotoText.png");
	emailtPhotoTextTex     = *AssetManager::getInstance()->getTexture("images/serverScreen/sendEmailText.png");	
	playMoreTex            = *AssetManager::getInstance()->getTexture("images/serverScreen/playMore.png");
	
	photoRamki().setup();
	socialPopup().setup();
	emailPopup().setup();
	qrCode.setup();

	touchKeyboard().setup(Vec2f(360.0f, getWindowHeight() - 504.0f));
	touchKeyboard().initKeyboard();
}

void ResultScreen::init( LocationEngine* game)
{
	_game = game;	

	_game->freezeLocation = false;
	canShowResultImages = false;
	isButtonsInit = false;
	isLeaveAnimation	= false;	

	qrCode.reset();
	socialPopup().reset();
	server().reset();	

	alphaSocialAnimate = 0.0f;
	alphaEmailAnimate = 0.0f;
	alphaFinAnimate = 0.0f;

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
		drawHandler = &ResultScreen::drawPhotoLoadingPreloader;
		state = INIT_STATE;
		photoLoadingFromDirSignal = photoMaker().photoLoadEvent.connect(boost::bind(&ResultScreen::photoLoadedFromDirHandler, this));	
		photoLoadingFromDirErrorSignal = photoMaker().photoLoadErrorEvent.connect(boost::bind(&ResultScreen::photoLoadeFromDirErrorHandler, this));	
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStartFinished, this ) );		
	}
	else
	{
		drawHandler = &ResultScreen::drawUpsetScreen;
		state = SORRY_GO_HOME;	
		comeBackSignal = comeBackBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
		comeBackTimerStart();
	}
}

void ResultScreen::animationStartFinished()
{
	drawHandler = &ResultScreen::drawPhotoLoadingPreloader;
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
	drawHandler = &ResultScreen::drawErrorScreen;
	state = ERROR_STATE;

	photoLoadingFromDirSignal.disconnect();
	photoLoadingFromDirErrorSignal.disconnect();
	comeBackSignal = comeBackBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
	comeBackTimerStart();
}

void ResultScreen::animationPhotoSavedFinished()
{
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
		drawHandler = &ResultScreen::drawNetConnectionPreloader;
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
		canShowResultImages = true;	
		photoRamki().initAnimationParams();
		_game->freezeLocation = false;	
		state = NET_OFF_LOCATION_READY;	
		connectButtons();
		comeBackTimerStart();	
	}
	else
	{
		_game->freezeLocation = true;	
		state = PHOTO_LOADING_TO_SERVER;
		drawHandler = &ResultScreen::drawServerPreloader;
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
	canShowResultImages = true;	
	photoRamki().initAnimationParams();	
	
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

	canShowResultImages = true;	
	photoRamki().initAnimationParams();	

	connectButtons();
	comeBackTimerStart();	
}

void ResultScreen::connectButtons()
{
	if(server().isConnected)
	{
		fbSignal =facebookBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::facebookBtnHandler, this));
		vkSignal =vkontakteBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::vkBtnHandler, this));		
	}

	mailSignal =  mailBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::openEmailBtnHandler, this));
	comeBackSignal = comeBackBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));

	isButtonsInit = true;


	if (alphaSocialAnimate <= 0.0f)
	timeline().apply( &alphaSocialAnimate, 1.0f, 0.9f, EaseOutCubic() );

	if (alphaEmailAnimate <= 0.0f)
	timeline().apply( &alphaEmailAnimate, 1.0f, 0.9f, EaseOutCubic() ).delay(0.4);

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
		drawHandler = &ResultScreen::drawEmailPopup;
		state = POPUP_EMAIL;	

		emailPopup().show();
		closeEmailPopupSignal = emailPopup().closeEvent.connect(boost::bind(&ResultScreen::closeEmailPopup, this));
		sendToMailSignal = emailPopup().sendEvent.connect(boost::bind(&ResultScreen::sendToEmailBtnHandler, this));
		disconnectButtons();
	}
	else if (type == popupTypes::VKONTAKTE || type == popupTypes::FACEBOOK)
	{
		drawHandler = &ResultScreen::drawSocialPopup;
		state = POPUP_MODE;	

		socialPopup().show(type);	
		disconnectButtons();
		closeSocialPopupSignal = socialPopup().closeEvent.connect(boost::bind(&ResultScreen::closeSocialPopup, this));
	}
}

void ResultScreen::drawEmailPopup()
{
	emailPopup().draw();
}

void ResultScreen::drawSocialPopup()
{
	socialPopup().draw();
}

void ResultScreen::closeEmailPopup()
{
	drawHandler= &ResultScreen::drawNothing;
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
	drawHandler= &ResultScreen::drawNothing;
	closeSocialPopupSignal.disconnect();
	socialPopup().disconnectAll();
	connectButtons();	
}

void ResultScreen::sendPhotoToEmail() 
{
	drawHandler = &ResultScreen::drawSendingToMailPreloader;
	state = PHOTO_SENDING_TO_MAIL;

	_game->freezeLocation = true;	
	timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowSendingToMailText, this ) );		
}

void ResultScreen::animationShowSendingToMailText() 
{	
	serverSignalLoadingEmailCheck = server().sendToMailEvent.connect( 
			boost::bind(&ResultScreen::serverLoadingEmailHandler, this) 
		);	

	server().sendToMail(emailPopup().getEmailsInString());
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

void ResultScreen::mouseEvents(int type )
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

		case POPUP_MODE:
			socialPopup().update();
		break;
	}
}

void ResultScreen::draw() 
{
	gl::enableAlphaBlending();

	(this->*drawHandler)();

	if(state!= POPUP_EMAIL && state!=POPUP_MODE)
	{
		drawResultImagesIfAllow();
		drawQRCodeIfAllow();	
		drawButtonsIfAllow();
	}

	drawFadeOutIfAllow();

	#ifdef debug
		if (!returnTimer.isStopped())
		{	
			string debugString = to_string(getSecondsToComeBack());
			Utils::textFieldDraw(debugString,  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));		
		}
	#endif

	gl::disableAlphaBlending();	
}

void ResultScreen::drawResultImagesIfAllow() 
{
	if(canShowResultImages)	
		photoRamki().draw();	
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
		Vec2f mailShift = Vec2f(0.0f, 0.0f);
		if(server().isConnected)
		{	
			gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaSocialAnimate));
			gl::draw(postPhotoTextTex, Vec2f(179.0f, 512.0f));
			facebookBtn->draw();
			vkontakteBtn->draw();
			gl::color(Color::white());
		}
		else
			mailShift = Vec2f(-511.0f, 0.0f);
	
		gl::pushMatrices();
			gl::translate(mailShift);
			gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaEmailAnimate));
			gl::draw(emailtPhotoTextTex, Vec2f(690.0f, 512.0f));
			mailBtn->draw();
			gl::color(Color::white());
		gl::popMatrices();

		comeBackBtn->draw();
	}
}

void ResultScreen::drawFadeOutIfAllow() 
{
	if (isLeaveAnimation)
	{
		gl::color(ColorA(BLUE.r, BLUE.g, BLUE.b, alphaFinAnimate));	
		gl::drawSolidRect(getWindowBounds());
		gl::color(ColorA(0,0,0,1));
	}
}

void ResultScreen::drawPhotoLoadingPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Выгружаю фотографии... "+ to_string(photoMaker().getElapsedSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void ResultScreen::drawNetConnectionPreloader() 
{
	gl::color(ColorA(1, 1, 1, alphaAnimate));
	Utils::textFieldDraw("Проверяю соединение... "+ to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void ResultScreen::drawServerPreloader() 
{	
	gl::color(ColorA(1, 1, 1, alphaAnimate));	
	Utils::textFieldDraw("Ожидаю сервер... " + to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void ResultScreen::drawSendingToMailPreloader() 
{	
	gl::color(ColorA(1, 1, 1, alphaAnimate));	
	Utils::textFieldDraw("Отправляю на почту... " + to_string(server().getTimeoutSeconds()),  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void ResultScreen::drawUpsetScreen() 
{
	Utils::textFieldDraw("Вы не угадали ни одной позы...(((",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	comeBackBtn->draw();
}

void ResultScreen::drawErrorScreen() 
{
	Utils::textFieldDraw("Что-то пошло не так...(((",  fonts().getFont("Helvetica Neue", 46), Vec2f(40.f, 40.0f), ColorA(1.f, 0.f, 0.f, 1.f));
	comeBackBtn->draw();
}

void ResultScreen::drawNothing() 
{

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
	socialPopup().disconnectAll();

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
#pragma warning(pop)