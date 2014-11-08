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

	bg  = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );

	Texture emailBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/mailBtn.png");
	mailBtn = new ButtonTex(emailBtnTex,  "E-mail");
	mailBtn->setScreenField(Vec2f(674.0f, 672.0f));
	mailBtn->setDownState(emailBtnTex);

	Texture backBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/playMore.png");
	comeBackBtn = new ButtonTex(backBtnTex,  "BACK");
	comeBackBtn->setScreenField(Vec2f(1578.0f, 655.0f));
	comeBackBtn->setDownState(backBtnTex);

	Texture comeBackBtnTex   = *AssetManager::getInstance()->getTexture( "images/diz/toStart.png" );
		
	backToStartBtn = new ButtonTex(comeBackBtnTex,  "backtoStart");
	backToStartBtn->setScreenField(Vec2f(0.0f, 958.0f));
	backToStartBtn->setDownState(comeBackBtnTex);

	Texture facebookBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/fb_off.png");
	Texture facebookBtnTexDown   = *AssetManager::getInstance()->getTexture("images/serverScreen/fb_on.png");	
	facebookBtn = new ButtonTex(facebookBtnTex,  "Facebook");
	facebookBtn->setScreenField(Vec2f(215.0f, 672.0f));
	facebookBtn->setDownState(facebookBtnTexDown);

	Texture vkontakteBtnTex   = *AssetManager::getInstance()->getTexture("images/serverScreen/vk_off.png");
	Texture vkontakteBtnTexDown   = *AssetManager::getInstance()->getTexture("images/serverScreen/vk_on.png");	
	vkontakteBtn = new ButtonTex(vkontakteBtnTex,  "Vkontakte");
	vkontakteBtn->setScreenField(Vec2f(215.0f, 840.0f));
	vkontakteBtn->setDownState(vkontakteBtnTexDown);

	postPhotoTextTex       = *AssetManager::getInstance()->getTexture("images/serverScreen/postPhotoText.png");
	emailtPhotoTextTex     = *AssetManager::getInstance()->getTexture("images/serverScreen/sendEmailText.png");	
	playMoreTex            = *AssetManager::getInstance()->getTexture("images/serverScreen/playMore.png");

	Texture playMoreTex1            = *AssetManager::getInstance()->getTexture("images/serverScreen/OneMore.png");
	comeBackBtn1 = new ButtonTex(playMoreTex1,  "BACK");
	comeBackBtn1->setScreenField(Vec2f(1449.0f, 692.0f));
	comeBackBtn1->setDownState(playMoreTex1);

	nothingCatTex           = *AssetManager::getInstance()->getTexture("images/serverScreen/nothingCat.png");

	photoRamki().setup();
	socialPopup().setup();
	emailPopup().setup();
	qrCode.setup();
	PlayerData::setup();

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

/*	#ifdef debug
		PlayerData::score = 3;
		PlayerData::playerData[0].pathHiRes = "level1.jpg";
		PlayerData::playerData[1].pathHiRes = "level2.jpg";
		PlayerData::playerData[2].pathHiRes = "level3.jpg";

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
	#endif*/	

	if(PlayerData::score != 0)
	{
		drawHandler = &ResultScreen::drawPhotoLoadingPreloader;
		state = INIT_STATE;
		photoLoadingFromDirSignal = photoMaker().photoLoadEvent.connect(boost::bind(&ResultScreen::photoLoadedFromDirHandler, this));	
		photoLoadingFromDirErrorSignal = photoMaker().photoLoadErrorEvent.connect(boost::bind(&ResultScreen::photoLoadeFromDirErrorHandler, this));	
		timeline().apply( &alphaAnimate, 0.0f, 0.2f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationStartFinished, this ) );		
	}
	else
	{
		drawHandler = &ResultScreen::drawUpsetScreen;
		state = SORRY_GO_HOME;	
		comeBackSignal1 = comeBackBtn1->mouseUpEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
		backToStartSignal = backToStartBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::backToStartHandler, this));
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
	bool isFBOCrashed = photoMaker().resizeFinalImages();

	if (isFBOCrashed == false)
		timeline().apply( &alphaAnimate, 0.0f, 0.2f, EaseOutCubic()).finishFn( bind( &ResultScreen::animationPhotoSavedFinished, this ) ).delay(0.3f);
	else
	{
		photoLoadeFromDirErrorHandler();
	}
}

void ResultScreen::photoLoadeFromDirErrorHandler()
{
	drawHandler = &ResultScreen::drawErrorScreen;
	state = ERROR_STATE;

	photoLoadingFromDirSignal.disconnect();
	photoLoadingFromDirErrorSignal.disconnect();
	comeBackSignal = comeBackBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
	backToStartSignal = backToStartBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::backToStartHandler, this));

	comeBackTimerStart();
}

void ResultScreen::animationPhotoSavedFinished()
{
	if (Params::isNetConnected == false)
	{
		state = NET_OFF_LOCATION_READY;	
		canShowResultImages = true;	
		photoRamki().initAnimationParams();
		_game->freezeLocation = false;	
		connectButtons();
		comeBackTimerStart();	
		drawHandler= &ResultScreen::drawNothing;
	}
	else
	{	
		_game->freezeLocation = true;
		drawHandler = &ResultScreen::drawNetConnectionPreloader;
		state = CHECKING_NET_CONNECTION;	
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.2f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowChekConnection, this ) );		
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
	timeline().apply( &alphaAnimate, 0.0f, 0.2f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationHideChekConnection, this ) );
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
		drawHandler= &ResultScreen::drawNothing;
	}
	else
	{
		_game->freezeLocation = true;	
		state = PHOTO_LOADING_TO_SERVER;
		drawHandler = &ResultScreen::drawServerPreloader;
		timeline().apply( &alphaAnimate, 0.0f, 1.0f, 0.2f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationShowServerPhotoLoad, this ) );	
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
	drawHandler= &ResultScreen::drawNothing;
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
	drawHandler= &ResultScreen::drawNothing;
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
	backToStartSignal = backToStartBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::backToStartHandler, this));

	isButtonsInit = true;


	if (alphaSocialAnimate <= 0.0f)
	timeline().apply( &alphaSocialAnimate, 1.0f, 0.9f, EaseOutCubic() );

	if (alphaEmailAnimate <= 0.0f)
	timeline().apply( &alphaEmailAnimate, 1.0f, 0.9f, EaseOutCubic() ).delay(0.4f);

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
		errorSavingEmailPopupSignal = emailPopup().errorSavingToBaseEvent.connect(boost::bind(&ResultScreen::errorSavingEmailHandler, this));		
		disconnectButtons();
	}
	else if (type == popupTypes::VKONTAKTE || type == popupTypes::FACEBOOK)
	{
		drawHandler = &ResultScreen::drawSocialPopup;
		state = POPUP_MODE;	

		socialPopup().show(type);	
		disconnectButtons();
		closeSocialPopupSignal = socialPopup().closeEvent.connect(boost::bind(&ResultScreen::closeSocialPopup, this));
		errorSavingEmailPopupSignal = emailPopup().errorSavingToBaseEvent.connect(boost::bind(&ResultScreen::errorSavingEmailHandler, this));		
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
	errorSavingEmailPopupSignal.disconnect();	
	emailPopup().disconnectAll();
	connectButtons();	
}

void ResultScreen::errorSavingEmailHandler()
{
	drawHandler = &ResultScreen::drawErrorScreen;
	state = ERROR_STATE;

	canShowResultImages = false;	
	isButtonsInit = false;

	closeEmailPopupSignal.disconnect();	
	errorSavingEmailPopupSignal.disconnect();	
	emailPopup().disconnectAll();
	
	comeBackSignal = comeBackBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::closeScreenHandler, this));
	backToStartSignal = backToStartBtn->mouseUpEvent.connect(boost::bind(&ResultScreen::backToStartHandler, this));

	comeBackTimerStart();
}

void ResultScreen::closeSocialPopup()
{
	state = DEFAULT_STATE;	
	drawHandler= &ResultScreen::drawNothing;
	closeSocialPopupSignal.disconnect();	
	socialPopup().disconnectAll();
	connectButtons();	
}

void ResultScreen::closeScreenHandler() 
{	
	if(!isLeaveAnimation)
	{
		disconnectListeners();
		_game->freezeLocation = true;
		isLeaveAnimation = true;
		timeline().apply( &alphaFinAnimate, 0.0f, 1.0f, 0.9f, EaseOutCubic() ).finishFn( bind( &ResultScreen::animationLeaveLocationFinished1, this ) );
	}
}

void ResultScreen::backToStartHandler() 
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

void ResultScreen::animationLeaveLocationFinished1() 
{
	_game->freezeLocation = false;
	_game->changeState(MainGameScreen::Instance());
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
		backToStartHandler();
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

	#ifdef drawTimer
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
			gl::draw(postPhotoTextTex, Vec2f(237.0f, 512.0f));
			facebookBtn->draw();
			vkontakteBtn->draw();
			gl::color(Color::white());
			mailBtn->setScreenField(Vec2f(701.0f, 672.0f));
		}
		else
		{
			mailShift = Vec2f(-482.0f, 0.0f);
			mailBtn->setScreenField(Vec2f(215.0f, 672.0f));
		}
	
		gl::pushMatrices();
			gl::translate(mailShift);
			gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaEmailAnimate));
			gl::draw(emailtPhotoTextTex, Vec2f(719.0f, 512.0f));			
		gl::popMatrices();

		mailBtn->draw();

		gl::color(Color::white());

		comeBackBtn->draw();

		backToStartBtn->draw();
	}
}

void ResultScreen::drawFadeOutIfAllow() 
{
	if (isLeaveAnimation)
	{
		//gl::color(ColorA(BLUE_FADE.r, BLUE_FADE.g, BLUE_FADE.b, alphaFinAnimate));	
		//gl::drawSolidRect(getWindowBounds());
		gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaFinAnimate));
		gl::draw(bg);
		gl::color(Color::white());
	}
}

void ResultScreen::drawPhotoLoadingPreloader() 
{
	//gl::color(ColorA(1, 1, 1, alphaAnimate));
	//Utils::textFieldDraw("Выгружаю фотографии... "+ to_string(photoMaker().getElapsedSeconds()),  fonts().getFont("MaestroC", 114), Vec2f(510.f, 448.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	Utils::textFieldDraw("Сохраняю фотографии",  fonts().getFont("MaestroC", 114), Vec2f(510.f, 448.0f), ColorA(1.f, 1.f, 1.f, 1.f));
}

void ResultScreen::drawNetConnectionPreloader() 
{
	//gl::color(ColorA(1, 1, 1, alphaAnimate));
	//Utils::textFieldDraw("Проверяю соединение... "+ to_string(server().getTimeoutSeconds()),  fonts().getFont("MaestroC", 114), Vec2f(510.f, 448.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	Utils::textFieldDraw("Сохраняю фотографии",  fonts().getFont("MaestroC", 114), Vec2f(510.f, 448.0f), ColorA(1.f, 1.f, 1.f, 1.f));

}

void ResultScreen::drawServerPreloader() 
{	
	//gl::color(ColorA(1, 1, 1, alphaAnimate));	
	//Utils::textFieldDraw("Ожидаю сервер... " + to_string(server().getTimeoutSeconds()),  fonts().getFont("MaestroC", 114), Vec2f(510.f, 448.0f), ColorA(1.f, 1.f, 1.f, 1.f));
	Utils::textFieldDraw("Сохраняю фотографии",  fonts().getFont("MaestroC", 114), Vec2f(510.f, 448.0f), ColorA(1.f, 1.f, 1.f, 1.f));

}

void ResultScreen::drawUpsetScreen() 
{
	gl::draw(nothingCatTex, Vec2f(162.0f, 64.0f));	
	comeBackBtn1->draw();
	backToStartBtn->draw();
}

void ResultScreen::drawErrorScreen() 
{
	Utils::textFieldDraw("Что-то пошло не так...\nЕсли ситуация повторится,\nперезапустите программу",  fonts().getFont("MaestroC", 114), Vec2f(510.f, 348.0f), Color::white());
	comeBackBtn->draw();
	backToStartBtn->draw();
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
	errorSavingEmailPopupSignal.disconnect();
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
	comeBackSignal1.disconnect();
	backToStartSignal.disconnect();
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