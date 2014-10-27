#pragma once

#include "cinder/Cinder.h"
#include "cinder/DataSource.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "ButtonColor.h"
#include "FontStore.h"
#include "Params.h"

#include <boost/thread.hpp>
#include <boost/functional/hash.hpp>
#include "VirtualKeyboard.h"
#include "AssetsManager.h"

#include "CinderAwesomium.h"
#include "SocialShare.h"

#include "Vkontakte.h"
#include "Facebook.h"


#define   SERVER_STATUS_NONE 1
#define   SERVER_STATUS_POSTING	2
#define   SERVER_STATUS_POST_READY	3
#define   SERVER_STATUS_POST_ERROR	4
#define   SERVER_STATUS_USER_REJECT 5


#define   POPUP_ANIMATION_STATE 1
#define   POPUP_READY_STATE		2
#define   POPUP_INIT_STATE		3
	
class PopupBase
{
	public:	
		// singleton implementation
		static PopupBase& getInstance() { static PopupBase popup; return popup; };

		void			setup();
		void			draw();
		void			update();
		void			reset();	
		void			show(int popuptype);
		void			disconnectAll();	
		bool			isDrawing;

		ci::gl::Texture		screenShot;

		boost::signals2::signal<void(void )> closeEvent;
		ci::signals::connection	closeBtnSignal, keyboardTouchSignal;

		ButtonColor			*closeBtn;

	private:		

		void	closedHandler();	
		void	initHandlers();
		void	keyboardTouchSignalHandler();
		void	hide();
		int		type;

		ci::Anim<ci::Vec2f>  bgPosition;
		ci::Anim<ci::ColorA> bgColor;
		ci::gl::Texture		*keyBoardMainBgTex;	

		SocShare *social;

		static Awesomium::WebCore*			mWebCorePtr;
		static Awesomium::WebView*			mWebViewPtr;
		static Awesomium::WebSession*		session;
		ci::gl::Texture						mWebTexture;

		ci::signals::connection				socialServerSignalCon;

		int									socialServerStatus;
		int popupAnimationState;

		void	socialServerSignal();
		bool isTryFocusInLoginTextField;

		std::string postingWaitingText;

		int									_facebookWindowHeight, _facebookWindowWidth;

		ci::Vec2f							_vkontakteOffset;
		ci::Vec2f							_facebookOffset;

		Area								facebookPopupAvailableArea, vkontaktePopupAvailableArea;

		std::vector<std::string>			photoURLs;
		
};

inline PopupBase&	socialPopup() { return PopupBase::getInstance(); };