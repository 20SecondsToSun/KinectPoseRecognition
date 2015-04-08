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
#include "EmailForm.h"

#include "GameScoreSaver.h"

using namespace ci;
using namespace ci::app;
using namespace ci::signals;
using namespace gl;
using namespace std;

class PhotoRamki;
class ResultScreen : public Location
{
public:
	void setup();
	void init( LocationEngine* game);
	void cleanup();
	void pause(){};
	void resume(){};
	void shutdown();

	void handleEvents(){};
	void mouseEvents(int type);
	void keyEvents(){};
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
	enum states {	INIT_STATE,
		SHOW_RESULT_PHOTO,
		PHOTO_LOADING_TO_SERVER,
		PHOTO_CREATE_COMICS, 
		PHOTO_LOADING_FROM_DIRECTORY,
		SERVER_EMAIL_ERROR,
		SERVER_INTERNET_ERROR,
		SORRY_GO_HOME,
		NET_OFF_LOCATION_READY,
		CHECKING_NET_CONNECTION,
		LOADING_TO_SERVER_SUCCESS,
		SAVING_LOCALY_SUCCESS,
		LOADING_TO_SERVER_FAIL,
		POPUP_MODE,			
		ERROR_STATE,
		POPUP_EMAIL,
		DEFAULT_STATE
	};

	void	animationLeaveLocationFinished();
	void	animationLeaveLocationFinished1();
	void	animationPhotoSavedFinished();
	void	animationStartFinished();
	void	animationShowChekConnection();
	void	animationHideChekConnection();
	void	animationShowServerPhotoLoad();
	void    animationHideServerPhotoLoad();

	void	drawPhotoLoadingPreloader();
	void	drawNetConnectionPreloader();
	void	drawUpsetScreen();
	void	showResultComics();
	void	drawErrorScreen();
	void	drawServerPreloader(); 
	void	drawPhotoRamka();

	void	drawEmailPopup();
	void	drawSocialPopup();
	void	drawNothing();
	void	drawResultImagesIfAllow();
	void	drawQRCodeIfAllow();
	void	drawButtonsIfAllow();
	void	drawFadeOutIfAllow();

	void    drawErrorStatus();

	void	photoLoadedFromDirHandler();
	void	photoLoadeFromDirErrorHandler();
	void	serverSignalConnectionCheckHandler();
	void	serverLoadingPhotoHandler();	
	void	serverLoadingEmailHandler();
	void	serverTimeoutHandler();
	void    errorSavingEmailHandler();

	void	connectButtons();
	void	disconnectButtons();
	void	disconnectListeners();

	void	initPopup(int);
	void	closeSocialPopup();
	void	closeEmailPopup();

	void	startLoadingProcess();

	void	resultPhotoRamkaStateInit();

	ButtonTex   *facebookBtn;
	ButtonTex   *vkontakteBtn;
	ButtonTex	*comeBackBtn, *comeBackBtn1;	
	ButtonTex	*mailBtn;	
	ButtonTex	*backToStartBtn;

	void	facebookBtnHandler();
	void	vkBtnHandler();
	void	openEmailBtnHandler();
	void	closeScreenHandler();
	void    backToStartHandler();
	void	sendToEmailBtnHandler();

	connection serverSignalLoadingCheck;
	connection serverSignalLoadingEmailCheck;
	connection photoLoadingFromDirSignal;
	connection photoLoadingFromDirErrorSignal;
	connection closeEmailPopupSignal;
	connection closeSocialPopupSignal;
	connection errorSavingEmailPopupSignal;
	connection backToStartSignal;

	connection	comeBackSignal, comeBackSignal1;
	connection	fbSignal;
	connection vkSignal;
	connection mailSignal;
	connection serverTimeoutCheck;
	connection sendToMailSignal;
	connection serverSignalConnectionCheck;

	Anim<float> scalePhotoRamkaAnimateVec;
	Anim<Vec2f> posPhotoRamkaAnimate;
	Anim<float> alphaAnimate, alphaFinAnimate;
	Anim<float> alphaSocialAnimate, alphaEmailAnimate, savingPhotopositionY;
	Anim<Vec2f> photoComicsPosition;

	bool canShowResultImages, isButtonsInit, isLeaveAnimation;

	float comicsPhotoScale[2];

	QRcode qrCode;

	Texture comicsPhotoVec[2];
	Texture	postPhotoTextTex, emailtPhotoTextTex;
	Texture	playMoreTex, nothingCatTex, bg, ramkaShadowTex, ramkaTex, savingPhotoTex;

	void (ResultScreen::* drawHandler)();
};