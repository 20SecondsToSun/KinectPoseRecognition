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
#include "PlayerData.h"

#include <boost/thread.hpp>
#include <boost/functional/hash.hpp>
#include "VirtualKeyboard.h"
#include "AssetsManager.h"

#include "CinderAwesomium.h"
#include "SocialShare.h"

#include "Vkontakte.h"
#include "Facebook.h"

#define   POPUP_ANIMATION_STATE 1
#define   POPUP_READY_STATE		2
#define   POPUP_INIT_STATE		3

namespace socialServerStatuses
{
	const int   WAITING_FOR_NETWORK 	 = 0;
	const int   SERVER_STATUS_NONE		 = 1;
	const int   POSTING					 = 2;
	const int   POST_READY = 3;
	const int   POST_ERROR = 4;
	const int   USER_REJECT= 5;
}
using namespace ci;
using namespace ci::gl;
using namespace std;

class PopupBase
{
public:	
	static PopupBase& getInstance() { static PopupBase popup; return popup; };

	void setup();
	void draw();
	void update();
	void reset();	
	void show(int popuptype);
	void shutdown();
	void disconnectAll();	
	bool isDrawing;

	Texture screenShot;

	boost::signals2::signal<void(void )> closeEvent;
	ci::signals::connection	closeBtnSignal, keyboardTouchSignal;

	ButtonTex *closeBtn;

private:

	void	closedHandler();	
	void	initHandlers();
	void	keyboardTouchSignalHandler();
	void	hide();
	int		type;

	void	setPhotoPaths();

	Anim<Vec2f>  bgPosition;
	Anim<ColorA> bgColor;
	Texture *keyBoardMainBgTex, *preloader, *blue_bg, *red_bg;	
	Texture *facebookOkTextTexture,*facebookErrTextTexture;
	Texture *vkontakteOkTextTexture,*vkontakteErrTextTexture;

	SocShare *social;

	static Awesomium::WebCore *mWebCorePtr;
	static Awesomium::WebView *mWebViewPtr;
	static Awesomium::WebSession *session;
	Texture mWebTexture;

	ci::signals::connection socialServerSignalCon;

	int	socialServerStatus;
	int popupAnimationState;

	void socialServerSignal();
	bool isTryFocusInLoginTextField;

	string postingWaitingText;

	int _facebookWindowHeight, _facebookWindowWidth;

	Vec2f _vkontakteOffset;
	Vec2f _facebookOffset;

	Area facebookPopupAvailableArea, vkontaktePopupAvailableArea;

	vector<string> photoURLs;

	void (PopupBase::* drawHandler)();
	void (PopupBase::* updateHandler)();

	void vkontakteDraw();
	void vkontakteUpdate();
	void facebookDraw();
	void facebookUpdate();

	void drawDef();
	void updateDef();
	void drawKeyboard();
	void drawcloseBtn();
	void drawPreloader();
	void drawPreloaderAtCenter();

	void drawFacebookPosted();
	void drawFacebookError();

	void drawVkontaktePosted();
	void drawVkontakteError();

	ci::signals::connection	KeyDownCon, MouseDownCon, MouseUpCon;
	void KeyDown( ci::app::KeyEvent event);
	void MouseDown( ci::app::MouseEvent &event);
	void MouseUp( ci::app::MouseEvent &event);
};

inline PopupBase&	socialPopup() { return PopupBase::getInstance(); };