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
	enum states {	INIT_STATE,
					PHOTO_LOADING_TO_SERVER,
					PHOTO_CREATE_COMICS, 
					PHOTO_LOADING_FROM_DIRECTORY,					
					SERVER_EMAIL_ERROR,
					SERVER_INTERNET_ERROR,
					SORRY_GO_HOME,
					NET_OFF_LOCATION_READY,
					CHECKING_NET_CONNECTION,
					LOADING_TO_SERVER_SUCCESS,
					LOADING_TO_SERVER_FAIL,
					POPUP_MODE,
					PHOTO_SENDING_TO_MAIL,
					ERROR_STATE
					};
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

protected:
	ResultScreen() { };

private:
	LocationEngine* _game;
	static ResultScreen ResultScreenState;

	int		state;
	bool	isChangingStateNow;	

	void	animationLeaveLocationFinished();
	//void	animationPhotoLoadedFinished();
	void	animationPhotoSavedFinished();
	void	animationStartFinished();
	//void	animationStart2Finished();
	void	animationStart2ServerLoadFinished();
	void	animationShowSendingToMailText();
	void    animationShowSendingToMailTextOut();
	

	void	drawPhotoMakerPreloader();
	void	drawPhotoLoadingPreloader();
	void	drawUpsetScreen();
	void	drawErrorScreen();
	void	drawServerPreloader(); 
	void	drawPopup();
	void	drawSendingToMailPreloader();

	void	drawResultImagesIfAllow();
	void	drawQRCodeIfAllow();
	void	drawButtonsIfAllow();
	void	drawFadeOutIfAllow();

	void	photoLoadedFromDirHandler();
	void	photoLoadeFromDirErrorHandler();
	void	serverSignalConnectionCheckHandler();
	void	serverLoadingPhotoHandler();	
	void	serverLoadingEmailHandler();	

	void	serverTimeoutHandler();

	void	connectButtons();
	void	disconnectButtons();
	void	disconnectListeners();

	void	initPopup(int);
	void	closePopup();

	ButtonColor	*mailBtn;
	ButtonColor *facebookBtn;
	ButtonColor *vkontakteBtn;
	ButtonColor *comeBackBtn;	

	void	facebookBtnHandler();
	void	vkBtnHandler();
	void	openEmailBtnHandler();
	void	closeScreenHandler();
	void	sendToEmailBtnHandler();
	
	ci::signals::connection serverSignalLoadingCheck;
	ci::signals::connection serverSignalLoadingEmailCheck;
	ci::signals::connection photoLoadingFromDirSignal;
	ci::signals::connection photoLoadingFromDirErrorSignal;
	ci::signals::connection closePopupSignal;

	ci::signals::connection	comeBackSignal;
	ci::signals::connection	fbSignal;
	ci::signals::connection vkSignal;
	ci::signals::connection mailSignal;
	ci::signals::connection serverTimeoutCheck;
	ci::signals::connection sendToMailSignal;
	ci::signals::connection serverSignalConnectionCheck;

	ci::Anim<float> alphaAnimate, alphaFinAnimate;
	ci::Anim<float> alphaAnimateComics[3];
	bool canShowResultImages, isButtonsInit, isLeaveAnimation;

	QRcode qrCode;		

	void	savePhotoToLocalBase();
	void	sendPhotoToEmail();
};