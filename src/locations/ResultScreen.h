#pragma once

#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"


#include "ButtonColor.h"
#include "PlayerData.h"
#include "Location.h"
#include "Params.h"
#include "IntroScreen.h"
#include "Server.h"
#include "QRcode.h"
#include "Popup.h"
#include "PhotoMaker.h"
#include "TextureManager.h"
#include "Saver.h"


using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace ReadyScreenDefaults
{
	//nst int  FONS_SCREENS_NUM					    = 6;
	enum states {	INIT_STATE,
					PHOTO_LOADING_TO_SERVER,
					PHOTO_CREATE_COMICS, 
					PHOTO_LOADING_FROM_DIRECTORY,
					PHOTO_LOADING,
					SERVER_EMAIL_ERROR,
					SERVER_INTERNET_ERROR,
					SORRY_GO_HOME,
					NET_OFF_LOCATION_READY,
					CHECKING_NET_CONNECTION,
					LOADING_TO_SERVER_SUCCESS,
					LOADING_TO_SERVER_FAIL,
					EMAIL_POPUP_MODE,
					FB_POPUP_MODE,
					VK_POPUP_MODE};
}

class ResultScreen : public Location
{

public:
	void setup();
	void init( LocationEngine* game);
	void cleanup();

	void pause();
	void resume();

	void shutdown();

	void handleEvents();
	void mouseEvents();
	void keyEvents();
	void update();
	void draw();

	static ResultScreen* Instance() {
		return &ResultScreenState;
	}

	void gotoFirstScreen();

	void serverSignal();

protected:
	ResultScreen() { };

private:
	LocationEngine*					_game;
	static ResultScreen				ResultScreenState;

	ButtonColor				*mailBtn, *facebookBtn, *vkontakteBtn, *comeBackBtn;
	
	Texture*						logoTexture, *helloTexture;	
	ci::Anim<float>					alphaFade;
	std::vector< ci::gl::Texture*>  fonImgVector;
	int								currentFon;

	ci::Surface						screnshot;

	//ReadyScreenDefaults::states		state;

	ci::Anim<float>					alphaFlash;
	ci::Anim<ci::Vec2f>				startPhotoScale, startPhotoXY;



	float							oneWidth;

	void							changeState();
	bool							isChangingStateNow;	

	void							animationFinished();
	

	int								state;

	void							drawPhotoMakerPreloader();
	void							drawPhotoLoadingPreloader();
	void							drawServerPreloader(); 
	void							drawPopup();
	void							photoLoaded();
	void							photoSaved();


	Server							server;
	ci::signals::connection			serverSignalLoadingCheck, serverSignalConnectionCheck, serverSignalLoadingEmailCheck, photoSaveSignal, photoLoadingSignal, closeSignal;
	ci::signals::connection			comeBackSignal, fbSignal, vkSignal, mailSignal;

	void							serverSignalConnectionCheckHandler();

	void							serverLoadingPhotoHandler();

	bool							isServer, isNetConnection;
	void							initButtons();

	
	void							facebookHandled();
	void							vkHandled();
	void							mailHandled();
	void							localPhotoSaveToBase();
	void							serverLoadingEmailHandler();


	ci::Anim<float>					alphaAnimate, alphaFinAnimate;
	ci::Anim<float>					alphaAnimateComics[3];
	bool							isControlsBlocked, canShowResultImages, isButtonsInit, goingOut;

	void							animationPhotoLoadedFinished();
	void							animationPhotoSavedFinished();
	void							animationStartFinished();
	void							animationStart2Finished();
	void							animationStart2ServerLoadFinished();


	void							disconnectListeners();

	QRcode							qrCode;


	void							drawResultImagesIfAllow();
	void							drawQRCodeIfAllow();
	void							drawButtonsIfAllow();
	void							drawFadeOutIfAllow();

	void							disconnectButtons();

	void							initPopup();
	void							closePopup();
	
};