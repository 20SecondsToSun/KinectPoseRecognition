#include "Popup.h"

using namespace ci;
using namespace Awesomium;
using namespace socialServerStatuses;

WebCore*		 PopupBase::mWebCorePtr;
WebView*		 PopupBase::mWebViewPtr;
WebSession*		 PopupBase::session;

void	PopupBase::setup()
{
	gl::Texture closeEmailTex = gl::Texture( loadImage( loadAsset("keyboard/closeEmail.png"  )));
	closeBtn= new ButtonTex(closeEmailTex, "closeEmail");
	closeBtn->setScreenField( Vec2f(1778.0f, 88.0f));	
	closeBtn->setDownState(closeEmailTex);

	facebookOkTextTexture   = AssetManager::getInstance()->getTexture("images/social/fb_ok.png" );
	facebookErrTextTexture  = AssetManager::getInstance()->getTexture("images/social/fb_err.png" );
	vkontakteOkTextTexture  = AssetManager::getInstance()->getTexture("images/social/vk_ok.png" );
	vkontakteErrTextTexture = AssetManager::getInstance()->getTexture("images/social/vk_err.png" );
	keyBoardMainBgTex       = AssetManager::getInstance()->getTexture(  "keyboard/06_podl.jpg" );	
	preloader = AssetManager::getInstance()->getTexture("images/social/preloader.png" );
	blue_bg   = AssetManager::getInstance()->getTexture("images/social/blue_bg.png" );
	red_bg    = AssetManager::getInstance()->getTexture("images/social/red_bg.png" );
	

	mWebCorePtr = WebCore::Initialize( WebConfig());	
	session     = mWebCorePtr->CreateWebSession(Awesomium::WSLit("soc"), WebPreferences());	
	mWebViewPtr = mWebCorePtr->CreateWebView( getWindowWidth(), getWindowHeight(), session);

	_vkontakteOffset			= Vec2f(0.0f, 1080.0f - 1754.0f + 674.0f);
	vkontaktePopupAvailableArea = Area(0,  110, getWindowWidth(), 550);
	facebookPopupAvailableArea  = Area(0, 50, getWindowWidth(), 550);

	_facebookWindowHeight = 500;	
	_facebookWindowWidth  = 700;

	photoURLs.clear();

	for (int i = 0; i < 3; i++)
	{
		string photo_url = "";
		string path =  Params::getTempPhotoSavePath(i).string();	
		
		for (size_t i = 0; i < path.size(); i++)
		{
			photo_url.push_back( path[i]);
			if (path[i] == '\\' ) photo_url.push_back( path[i]);
		}	
		console()<< "url::  "<<photo_url<<endl;
		photoURLs.push_back(photo_url);		
	}

	//touchKeyboard().setup(Vec2f(360.0f, getWindowHeight() - 504.0f));
	//touchKeyboard().initKeyboard();

	drawHandler  = &PopupBase::drawDef;
	updateHandler = &PopupBase::updateDef;
}

void PopupBase::reset()
{
	isDrawing = false;	
}

void PopupBase::show(int _type)
{	
	socialServerStatus  = WAITING_FOR_NETWORK;
	type = _type;
	screenShot = gl::Texture(ci::app::copyWindowSurface());	
	session->ClearCookies();

	isTryFocusInLoginTextField = false;	
	isDrawing  = true;	

	//touchKeyboard().setPosition( Vec2f(360.0f, getWindowHeight() - 504.0f));

	bgPosition =  Vec2f(0.0f, 0.0f);
	bgColor =  ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	timeline().apply( &bgPosition, Vec2f(0.0f,  getWindowHeight() - 1754.0f), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &PopupBase::initHandlers, this ) );
	timeline().apply( &bgColor,  ColorA(1.0f, 1.0f, 1.0f, 0.98f), 0.5f, EaseOutQuart());


	if (type == popupTypes::VKONTAKTE)
	{	
		social = new Vkontakte();		
		social->clear_token();	
		socialServerSignalCon = social->serverHandler.connect( 
			boost::bind(&PopupBase::socialServerSignal, this) 
		);		

		postingWaitingText = "Отправляем фотографии во ВКонтакте..";

		touchKeyboard().setPosition( Vec2f(360.0f, 1080.0f - 484.0f));

		if( mWebViewPtr )
			mWebViewPtr->Resize(getWindowWidth(), 550 );	
		
		mWebViewPtr->LoadURL(  WebURL(WSLit( social->getAuthUrl())));
		mWebViewPtr->Focus();

		drawHandler  = &PopupBase::vkontakteDraw;
		updateHandler = &PopupBase::vkontakteUpdate;
	}
	else if (type == popupTypes::FACEBOOK)
	{		
		social = new Facebook();	
		social->clear_token();
		socialServerSignalCon = social->serverHandler.connect( 
			boost::bind(&PopupBase::socialServerSignal, this) 
		);
	
		postingWaitingText = "Отправляем фотографии в Facebook..";

		touchKeyboard().setPosition( Vec2f(360.0f, 1080.0f - 484.0f));

		if( mWebViewPtr )
			mWebViewPtr->Resize(_facebookWindowWidth, _facebookWindowHeight );

		_facebookOffset = Vec2f(0.5f * (getWindowWidth() - _facebookWindowWidth), 1080.0f - 1754.0f+674.0f+20.0f);
	
		mWebViewPtr->LoadURL( WebURL( WSLit( social->getAuthUrl())));
		mWebViewPtr->Focus();

		drawHandler  = &PopupBase::facebookDraw;
		updateHandler = &PopupBase::facebookUpdate;
	}
}

void PopupBase::update()
{
	mWebCorePtr->Update();

	if(mWebViewPtr && ph::awesomium::isDirty( mWebViewPtr ) ) 
	{		
		try
		{
			gl::Texture::Format fmt; 
			fmt.setMagFilter( GL_NEAREST );
			mWebTexture = ph::awesomium::toTexture( mWebViewPtr, fmt );			
		}
		catch( const std::exception &e ) {
			console() << e.what() << std::endl;
		}

		char title[1024];
		mWebViewPtr->title().ToUTF8( title, 1024 );
	}

	if(mWebViewPtr)
			(this->*updateHandler)();
}

void PopupBase::draw()
{
	(this->*drawHandler)();		
}

void PopupBase::initHandlers()
{
	keyboardTouchSignal = touchKeyboard().keyboardTouchSignal.connect(boost::bind(&PopupBase::keyboardTouchSignalHandler, this));
	closeBtnSignal =  closeBtn->mouseDownEvent.connect(boost::bind(&PopupBase::hide, this));

	MouseDownCon   = getWindow()->getSignalMouseDown().connect( std::bind( &PopupBase::MouseDown, this, std::placeholders::_1 ) );
	MouseUpCon	   = getWindow()->getSignalMouseUp().connect(   std::bind( &PopupBase::MouseUp,   this, std::placeholders::_1 ) );
	KeyDownCon	   = getWindow()->getSignalKeyDown().connect(   std::bind( &PopupBase::KeyDown,	  this, std::placeholders::_1 ) );		
}

void PopupBase::socialServerSignal()
{
	if (social->getResponse() == "OK")
		socialServerStatus = POST_READY;
	else 
		socialServerStatus = POST_ERROR;
}

void PopupBase::keyboardTouchSignalHandler()
{
	if ( touchKeyboard().isBackCode())
	{	
		if(mWebViewPtr->focused_element_type()!= 3 && 
		   mWebViewPtr->focused_element_type()!= 4 ) return;

		KeyEvent key = VirtualKeyboard::imitate_BACKSPACE_KEY_EVENT();
		ph::awesomium::handleKeyDown( mWebViewPtr, key );
	}
	else if ( touchKeyboard().isMailCode())
	{
		string lastCode =  touchKeyboard().getLastCode();

		for (size_t i = 0; i < lastCode.size(); i++)
		{
			char chr = lastCode[i];		
			chr = tolower(chr);
			KeyEvent key(getWindow(),1,chr,chr, 1,chr);
			ph::awesomium::handleKeyDown( mWebViewPtr, key );
		}		
	}
	else if ( touchKeyboard().isSendCode())
	{
		KeyEvent key = VirtualKeyboard::imitate_ENTER_KEY_EVENT();
		ph::awesomium::handleKeyDown( mWebViewPtr, key );		
	}
	else
	{
		string lastCode =  touchKeyboard().getLastCode();
		char chr = lastCode[0];			
		KeyEvent key(getWindow(),1,chr,chr, 1,chr);
		ph::awesomium::handleKeyDown( mWebViewPtr, key );
	}
}

void PopupBase::hide()
{
	disconnectAll();
	timeline().apply( &bgPosition,Vec2f(0.0f, 0.0f), 0.6f, EaseInQuart()).finishFn( bind( &PopupBase::closedHandler, this ) );		
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.1f), 0.5f, EaseInQuart()).delay(0.4f);
}

void	PopupBase::closedHandler()
{
	//disconnectAll();
	closeEvent();
}

void PopupBase::disconnectAll()
{
	closeBtnSignal.disconnect();
	keyboardTouchSignal.disconnect();


	MouseUpCon.disconnect( );
	MouseDownCon.disconnect();
	KeyDownCon.disconnect();

	isDrawing = false;	
}

void PopupBase::MouseDown( MouseEvent &event )
{
	if (socialServerStatus == POST_READY || socialServerStatus == POST_ERROR)
	{
		hide();
		return;
	}

	if (type == popupTypes::VKONTAKTE && vkontaktePopupAvailableArea.contains(event.getPos()))
	{			
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_vkontakteOffset);
		ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );		
	}
	else if (type == popupTypes::FACEBOOK && facebookPopupAvailableArea.contains(event.getPos()))
	{	
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_facebookOffset);
		ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );	
	}

	console()<< "  coord  :: "<< event.getPos()<<endl;
}

void PopupBase::MouseUp( MouseEvent &event )
{
	if (type == popupTypes::VKONTAKTE && 
		vkontaktePopupAvailableArea.contains(event.getPos()))
	{					
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_vkontakteOffset);
		ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );	
	}
	else if (type == popupTypes::FACEBOOK && 
			facebookPopupAvailableArea.contains(event.getPos()))
	{		
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_facebookOffset);
		ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );		
	}
}

void PopupBase::KeyDown( KeyEvent event )
{	
	ph::awesomium::handleKeyDown( mWebViewPtr, event );
}

void PopupBase::vkontakteUpdate()
{
	if(socialServerStatus == USER_REJECT || socialServerStatus == POSTING || socialServerStatus == POST_READY || socialServerStatus == POST_ERROR)
		return;

	char anchr[1024];
	mWebViewPtr->url().anchor().ToUTF8( anchr, 1024 );
	string anchString( anchr );
	size_t pos = anchString.find("access"); 
		
	if (pos==0 && socialServerStatus == WAITING_FOR_NETWORK) 	
	{	
		string delimiter = "&";
		string token = anchString.substr(0, anchString.find(delimiter));
		social->access_token = token.substr(13);
		socialServerStatus  = POSTING;

		//social->postStatus( "#ТойотаНастроение тест");
		social->postPhoto(photoURLs, "#Котопоза");
	}
	else 
	{
		size_t pos_denied = anchString.find("User denied your request"); 
		if (pos_denied <1000)
		{
			socialServerStatus = USER_REJECT;
			hide();
		}		
	}

	if(!isTryFocusInLoginTextField && mWebTexture && mWebViewPtr->IsLoading() == false )
	{
		MouseEvent mEvent= VirtualKeyboard::inititateMouseEvent(Vec2f(932.0f, 246.0f));	
		isTryFocusInLoginTextField = true;
		ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );
		ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );
	}

}
void PopupBase::vkontakteDraw()
{
	if (socialServerStatus == POSTING)
	{
		drawPreloaderAtCenter();
		return;
	}
	else if (socialServerStatus == POST_READY)
	{
		gl::color(bgColor);
		drawVkontaktePosted();
		return;
	}
	else if (socialServerStatus == POST_ERROR)
	{
		gl::color(bgColor);
		drawVkontakteError();
		return;
	}

	drawKeyboard();

	if( mWebTexture  && !mWebViewPtr->IsLoading() && socialServerStatus != USER_REJECT)
	{
		gl::pushMatrices();			
			gl::translate(bgPosition);
			gl::translate(0.0f, 674.0f);		
			gl::translate(0.5f*(getWindowWidth() - mWebTexture.getWidth()), 0.0f);
			gl::color(bgColor);
			gl::draw( mWebTexture );
		gl::popMatrices();
	}
	else
	{
		drawPreloader();
	}

	//gl::drawSolidRect(vkontaktePopupAvailableArea);

	drawcloseBtn();
}


void PopupBase::facebookUpdate()
{
	if(socialServerStatus == USER_REJECT || socialServerStatus == POSTING || socialServerStatus == POST_READY || socialServerStatus == POST_ERROR)
		return;

	char anchr[1024];
	mWebViewPtr->url().anchor().ToUTF8( anchr, 1024 );
	string anchString( anchr );
	size_t pos = anchString.find("access_token"); 		
		
	if (pos == 0 && socialServerStatus == WAITING_FOR_NETWORK) 	
	{	
		string delimiter		= "&";
		string token			= anchString.substr(0, anchString.find(delimiter)); 
		social->access_token		=  token.substr(13);				
		socialServerStatus			= POSTING;		
			
		social->postPhoto(photoURLs, "#Котопоза");
		//social->postStatus( "#ТойотаНастроение");
	}	
	else 
	{
		char query[1024];
		mWebViewPtr->url().query().ToUTF8( query, 1024 );
		string queryString( query );
		size_t pos_denied = queryString.find("error_reason=user_denied"); 
		if (queryString.size() == 0 ||pos_denied <1000)
		{
			socialServerStatus = USER_REJECT;
			hide();
		}
	}

	if(!isTryFocusInLoginTextField && mWebTexture && mWebViewPtr->IsLoading() == false )
	{
		MouseEvent mEvent= VirtualKeyboard::inititateMouseEvent(Vec2f(947.0f, 118.0f));
		isTryFocusInLoginTextField = true;
		ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );
		ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );
	}
}

void PopupBase::facebookDraw()
{
	if (socialServerStatus == POSTING)
	{
		drawPreloaderAtCenter();
		return;
	}
	else if (socialServerStatus == POST_READY)
	{
		gl::color(bgColor);
		drawFacebookPosted();
		return;
	}
	else if (socialServerStatus == POST_ERROR)
	{
		gl::color(bgColor);
		drawFacebookError();
		return;
	}

	drawKeyboard();

	if( mWebTexture  && !mWebViewPtr->IsLoading() && socialServerStatus != USER_REJECT)
	{
		gl::pushMatrices();			
			gl::translate(bgPosition);
			gl::translate(0.0f, 674.0f);		
			gl::color( Color::hex(0x4e4e4e) );
			gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), _facebookWindowHeight + 40.0f));	
			gl::translate(0.5f*(getWindowWidth() - mWebTexture.getWidth()), 20.0f);
			gl::color(bgColor);			
			gl::draw( mWebTexture );
		gl::popMatrices();		
	}
	else
	{
		drawPreloader();
	}

	drawcloseBtn();
}

void PopupBase::drawFacebookPosted()
{
	gl::draw(*blue_bg);
	gl::draw(*facebookOkTextTexture, Vec2f((getWindowWidth() - 891.0f)*0.5f, (getWindowHeight() - 564.0f)*0.5f ));
}

void PopupBase::drawFacebookError()
{
	gl::draw(*red_bg);
	gl::draw(*facebookErrTextTexture, Vec2f((getWindowWidth() - 711.0f)*0.5f, (getWindowHeight() - 541.0f)*0.5f ));
}

void PopupBase::drawVkontaktePosted()
{
	gl::draw(*blue_bg);
	gl::draw(*vkontakteOkTextTexture, Vec2f((getWindowWidth() - 891.0f)*0.5f, (getWindowHeight() - 564.0f)*0.5f ));	
}

void PopupBase::drawVkontakteError()
{
	gl::draw(*red_bg);
	gl::draw(*vkontakteErrTextTexture, Vec2f((getWindowWidth() -711.0f)*0.5f, (getWindowHeight() - 541.0f)*0.5f ));
}

void PopupBase::drawKeyboard()
{
	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::color(bgColor);
		gl::draw(*keyBoardMainBgTex);
		gl::translate(0.0f, 674.0f);
		touchKeyboard().draw();		
	gl::popMatrices();
}

void PopupBase::drawcloseBtn()
{
	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::color(bgColor);		
		gl::translate(0.0f, 674.0f);
		closeBtn->draw();	
	gl::popMatrices();
}

void PopupBase::drawPreloader()
{
	gl::pushMatrices();
		gl::color(bgColor);
		gl::translate(bgPosition);
		gl::translate(950.0f, 674.0f+150.0f);
		gl::pushModelView();	
		gl::scale( 0.5f, 0.5f );
		gl::rotate( 180.0f * float( getElapsedSeconds() ) );
		gl::translate( -0.5f * Vec2f(151.0f, 151.0f ) );		
		gl::color( Color::white() );
		gl::draw( *preloader );
		gl::popModelView();
	gl::popMatrices();
}

void PopupBase::drawPreloaderAtCenter()
{
	gl::draw(*blue_bg);	
	gl::Texture text= Utils::getTextField(postingWaitingText, fonts().getFont("Myriad Pro", 70), Color::white());
	gl::draw(text, Vec2f(0.5f*(getWindowWidth() - text.getWidth()), 375.0f));

	gl::pushMatrices();
		gl::color(bgColor);
		gl::translate(bgPosition);
		gl::translate(950.0f, 974.0f+250.0f);
		gl::pushModelView();	
		gl::scale( 0.5f, 0.5f );
		gl::rotate( 180.0f * float( getElapsedSeconds() ) );
		gl::translate( -0.5f * Vec2f(151.0f, 151.0f ) );		
		gl::color( Color::white() );
		gl::draw( *preloader );
		gl::popModelView();
	gl::popMatrices();
}

void PopupBase::updateDef()
{
}

void PopupBase::drawDef()
{
}