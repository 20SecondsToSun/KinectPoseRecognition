#include "Popup.h"

using namespace ci;

Awesomium::WebCore*		 PopupBase::mWebCorePtr;
Awesomium::WebView*		 PopupBase::mWebViewPtr;
Awesomium::WebSession*   PopupBase::session;

void	PopupBase::setup()
{
	closeBtn		  = new ButtonColor(Rectf(1700,100, 1900, 200), Color(1,0,0), fonts().getFont("Helvetica Neue", 26),	"ЗАКРЫТЬ");	
	keyBoardMainBgTex = AssetManager::getInstance()->getTexture(  "keyboard/06_podl.jpg" );	

	// set Awesomium logging to verbose
	//Awesomium::WebConfig cnf;
	//cnf.log_level = Awesomium::kLogLevel_Verbose;
	//Awesomium::WebPreferences pref;	
	// initialize the Awesomium web engine
	mWebCorePtr		= Awesomium::WebCore::Initialize( Awesomium::WebConfig() );	
	//session			= mWebCorePtr->CreateWebSession(Awesomium::WSLit("soc"), pref);
	console()<<"--------start----------------"<<endl;
	mWebViewPtr		= mWebCorePtr->CreateWebView( 500, 500);//, session );	
	console()<<"--------fin----------------  "<<mWebViewPtr<<endl;

	_vkontakteOffset			= Vec2f(0.0f, 1080.0f - 1754.0f + 674.0f);
	vkontaktePopupAvailableArea = Area(0,  170, getWindowWidth(), 550);
	facebookPopupAvailableArea  = Area(746, 92, getWindowWidth(), 750);

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
		photoURLs.push_back(photo_url);		
	}
}

void	PopupBase::reset()
{
	isDrawing = false;	
}

void PopupBase::show(int _type)
{
	socialServerStatus  = SERVER_STATUS_NONE;
	isTryFocusInLoginTextField = false;

	type = _type;
	isDrawing  = true;
	screenShot = gl::Texture(ci::app::copyWindowSurface());	

	touchKeyboard().setPosition( Vec2f(360.0f, getWindowHeight() - 504.0f));

	bgPosition =  Vec2f(0.0f, 0.0f);
	bgColor =  ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	timeline().apply( &bgPosition, Vec2f(0.0f,  getWindowHeight() - 1754.0f), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &PopupBase::initHandlers, this ) );
	timeline().apply( &bgColor,  ColorA(1.0f, 1.0f, 1.0f, 0.98f), 0.5f, EaseOutQuart());


	if (type == popupTypes::VKONTAKTE)
	{	
		console()<<" init Vkontakte "<<endl;
		social = new Vkontakte();		
		social->clear_token();	
		socialServerSignalCon = social->serverHandler.connect( 
			boost::bind(&PopupBase::socialServerSignal, this) 
		);		

		postingWaitingText = "Отправляем фотографии во ВКонтакте..";

		//touchKeyBoard.setPosition( Vec2f(360.0f, HEIGHT - 484.0f));

		//if( mWebViewPtr )
		//	mWebViewPtr->Resize(getWindowWidth(), 550 );
	
		console()<<" auth url :: "<<social->getAuthUrl()<<std::endl;
		Awesomium::WebURL url( Awesomium::WSLit( "http://www.google.ru") );
		console()<<" ------------- "<<std::endl;
		mWebViewPtr->LoadURL( url);
		console()<<" ------------- "<<std::endl;		
		mWebViewPtr->Focus();
		console()<<" ------------- 3"<<std::endl;		
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
	
		console()<<" auth url :: "<<social->getAuthUrl()<<std::endl;
		mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( social->getAuthUrl()) ) );
		mWebViewPtr->Focus();
		
	}
}

void PopupBase::initHandlers()
{
	keyboardTouchSignal = touchKeyboard().keyboardTouchSignal.connect(boost::bind(&PopupBase::keyboardTouchSignalHandler, this));
	closeBtnSignal =  closeBtn->mouseDownEvent.connect(boost::bind(&PopupBase::hide, this));

	popupAnimationState = POPUP_READY_STATE;
	//touchKeyBoard.initKeyboard();
}


void PopupBase::socialServerSignal()
{
	if (social->getResponse() == "OK")
		socialServerStatus = SERVER_STATUS_POST_READY;
	else 
		socialServerStatus = SERVER_STATUS_POST_ERROR;
}

void PopupBase::keyboardTouchSignalHandler()
{
	if ( touchKeyboard().isBackCode())
	{	
		// if focus not in textInput return else means goint to previous web page
		if(mWebViewPtr->focused_element_type()!=3 && 
		   mWebViewPtr->focused_element_type()!=4 ) return;

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
	timeline().apply( &bgPosition,Vec2f(0.0f, 0.0f), 0.6f, EaseInQuart()).finishFn( bind( &PopupBase::closedHandler, this ) );		
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.4f), 0.5f, EaseInQuart()).delay(0.4f);
}

void	PopupBase::closedHandler()
{
	disconnectAll();
	closeEvent();
}

void PopupBase::disconnectAll()
{
	closeBtnSignal.disconnect();
	keyboardTouchSignal.disconnect();
	isDrawing = false;	
}













void PopupBase::update()
{
	if (socialServerStatus == SERVER_STATUS_USER_REJECT) return;

	// update the Awesomium engine
	if(mWebCorePtr)
	{		
		mWebCorePtr->Update();
	}
	console()<<" mWebCorePtr  "<<mWebCorePtr<<"  ph::awesomium::isDirty( mWebViewPtr )  "<< ph::awesomium::isDirty( mWebViewPtr )<<endl;
	// create or update our OpenGL Texture from the webview
	if(mWebViewPtr && ph::awesomium::isDirty( mWebViewPtr ) ) 
	{
		console()<<" try  "<<endl;
		try {
			// set texture filter to NEAREST if you don't intend to transform (scale, rotate) it
			gl::Texture::Format fmt; 
			fmt.setMagFilter( GL_NEAREST );

			// get the texture using a handy conversion function
			mWebTexture = ph::awesomium::toTexture( mWebViewPtr, fmt );
			console()<<"UPDATE TEXTURE!!"<<std::endl;
		}
		catch( const std::exception &e ) {
			console() << e.what() << std::endl;
		}

		// update the window title to reflect the loaded content
		char title[1024];
		mWebViewPtr->title().ToUTF8( title, 1024 );
	}

	if(mWebViewPtr)
	{
		if (type == popupTypes::FACEBOOK)
		{		
			char anchr[1024];
			mWebViewPtr->url().anchor().ToUTF8( anchr, 1024 );
			std::string anchString( anchr );
			std::size_t pos = anchString.find("access_token"); 		
			ci::app::console()<<"  anchString "<<anchString<<std::endl;

			if (pos == 0 && socialServerStatus == SERVER_STATUS_NONE) 	
			{	
				std::string delimiter		= "&";
				std::string token			= anchString.substr(0, anchString.find(delimiter)); 
				social->access_token		=  token.substr(13);				
				socialServerStatus			= SERVER_STATUS_POSTING;		
			
				social->postPhoto(photoURLs, "#Котопоза");
				//social->postStatus( "#ТойотаНастроение");
			}	
			else
			{
				char query[1024];
				mWebViewPtr->url().query().ToUTF8( query, 1024 );
				std::string queryString( query );
				std::size_t pos_denied = queryString.find("error_reason=user_denied"); 
				if (pos_denied <1000)
				{
					socialServerStatus = SERVER_STATUS_USER_REJECT;
					//removeHandlers();
					//hide();				
				}
			}			
		}	
		else if (type == popupTypes::VKONTAKTE)
		{
			char anchr[1024];
			mWebViewPtr->url().anchor().ToUTF8( anchr, 1024 );
			std::string anchString( anchr );
			std::size_t pos = anchString.find("access"); 
			console()<<" socialServerStatus "<<pos<<" -- "<<socialServerStatus<<endl;
			if (pos==0 && socialServerStatus == SERVER_STATUS_NONE) 	
			{	
				std::string delimiter = "&";
				std::string token = anchString.substr(0, anchString.find(delimiter));
				social->access_token = token.substr(13);
				socialServerStatus  = SERVER_STATUS_POSTING;

				//social->postStatus( "#ТойотаНастроение тест");
				social->postPhoto(photoURLs, "#Котопоза");
			}
			else 
			{
				std::size_t pos_denied = anchString.find("User denied your request"); 
				if (pos_denied <1000)
				{
					socialServerStatus = SERVER_STATUS_USER_REJECT;
					//removeHandlers();
					hide();
				}			
			}
		}
	}

	if (!isTryFocusInLoginTextField)
	{
		if( mWebTexture&& mWebViewPtr->IsLoading() == false )
		{
			MouseEvent mEvent;
			Vec2f coords;

			if (type == popupTypes::VKONTAKTE)
			{
				coords = Vec2f(932.0f, 246.0f);
				mEvent = VirtualKeyboard::inititateMouseEvent(coords);

			}
			else if (type == popupTypes::FACEBOOK)
			{
				coords = Vec2f(886.0f, 113.0f);
				mEvent = VirtualKeyboard::inititateMouseEvent(coords);
			}

			isTryFocusInLoginTextField = true;
			ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );
			ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );			
		}
	}
}

void PopupBase::draw()
{
	console()<<"popupAnimationState "<< mWebTexture<<endl;
	
	if (SERVER_STATUS_POSTING == socialServerStatus)
	{		
		//PopupMail::draw("SERVER_LOADING", postingWaitingText);
		return;
	}
	else if (SERVER_STATUS_POST_READY == socialServerStatus)
	{
		if (type == popupTypes::VKONTAKTE)
		{
			//PopupMail::draw("SERVER_OK_VK");
		}
		else if (type == popupTypes::FACEBOOK)
		{	//PopupMail::draw("SERVER_OK_FB");
		}
		return;
	}
	else if (SERVER_STATUS_POST_ERROR == socialServerStatus)
	{
		
		if (type == popupTypes::VKONTAKTE)
		{
			//PopupMail::draw("SERVER_ERROR_VK");
		}
		else  if (type == popupTypes::FACEBOOK)
		{
			//PopupMail::draw("SERVER_ERROR_FB");	
		}
		return;
	}

	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::color(bgColor);
		gl::draw(*keyBoardMainBgTex);
	//	gl::pushMatrices();
	//		if (type == popupTypes::VKONTAKTE || type == popupTypes::FACEBOOK)
	//		{
	//			//gl::translate(360.0f, 1270.0f);
	//		}		
	//		touchKeyboard().draw();
	//	gl::popMatrices();	
		gl::translate(0.0f, 674.0f);
		touchKeyboard().draw();
		closeBtn->draw();	
	gl::popMatrices();


	console()<<" mWebTexture  "<<mWebTexture<<"  mWebViewPtr->IsLoading() "<<mWebViewPtr->IsLoading()<<endl;

	if( popupAnimationState == POPUP_READY_STATE && mWebTexture && mWebViewPtr->IsLoading() == false )
	{		
		gl::pushMatrices();			
			gl::translate(bgPosition);
			gl::translate(0.0f, 674.0f);	
			if (type == popupTypes::FACEBOOK)
			{
				gl::color( Color::hex(0x4e4e4e) );
				gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), _facebookWindowHeight + 40.0f));	
				gl::translate(0.5f*(getWindowWidth() - mWebTexture.getWidth()), 20.0f);				
			}
			gl::color(bgColor);
			
			gl::draw( mWebTexture );
		gl::popMatrices();		
	}
	else
	{
		gl::pushMatrices();
			gl::color(bgColor);
			gl::translate(bgPosition);
			gl::translate(950.0f, 674.0f+150.0f);
			//PreloaderCircle::draw();
		gl::popMatrices();
	}

	gl::pushMatrices();
		gl::translate(bgPosition);		
		gl::translate(Vec2f(0.0f, 674.0f));	
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
		//closePopup.draw();
	gl::popMatrices();
}