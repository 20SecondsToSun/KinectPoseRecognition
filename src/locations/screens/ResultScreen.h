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
#include "EmailForm.h"

using namespace ci;
using namespace ci::app;
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
					PHOTO_SENDING_TO_MAIL,
					ERROR_STATE,
					POPUP_EMAIL,
					DEFAULT_STATE
				};

		void	animationLeaveLocationFinished();	
		void	animationPhotoSavedFinished();
		void	animationStartFinished();
		void	animationShowChekConnection();
		void	animationHideChekConnection();
		void	animationShowServerPhotoLoad();
		void    animationHideServerPhotoLoad();
		void	animationShowSendingToMailText();
		void    animationShowSendingToMailTextOut();
	
		void	drawPhotoLoadingPreloader();
		void	drawNetConnectionPreloader();
		void	drawUpsetScreen();
		void	drawErrorScreen();
		void	drawServerPreloader(); 
		void	drawSendingToMailPreloader();
		void	drawEmailPopup();
		void	drawSocialPopup();

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
		void	closeSocialPopup();
		void	closeEmailPopup();

		ButtonTex   *facebookBtn;
		ButtonTex   *vkontakteBtn;
		ButtonTex	*comeBackBtn;	
		ButtonTex	*mailBtn;			

		void	facebookBtnHandler();
		void	vkBtnHandler();
		void	openEmailBtnHandler();
		void	closeScreenHandler();
		void	sendToEmailBtnHandler();
	
		ci::signals::connection serverSignalLoadingCheck;
		ci::signals::connection serverSignalLoadingEmailCheck;
		ci::signals::connection photoLoadingFromDirSignal;
		ci::signals::connection photoLoadingFromDirErrorSignal;
		ci::signals::connection closeEmailPopupSignal;
		ci::signals::connection closeSocialPopupSignal;

		ci::signals::connection	comeBackSignal;
		ci::signals::connection	fbSignal;
		ci::signals::connection vkSignal;
		ci::signals::connection mailSignal;
		ci::signals::connection serverTimeoutCheck;
		ci::signals::connection sendToMailSignal;
		ci::signals::connection serverSignalConnectionCheck;

		ci::Anim<float> alphaAnimate, alphaFinAnimate;		
		bool canShowResultImages, isButtonsInit, isLeaveAnimation;

		QRcode qrCode;	

		Texture	postPhotoTextTex, emailtPhotoTextTex, downloadPhotoTextTex;
		Texture	plashkaTex, playMoreTex;	

		void	savePhotoToLocalBase();
		void	sendPhotoToEmail();			

		void (ResultScreen::* drawHandler)();
};

class PhotoRamki 
{
	public:

		// singleton implementation
		static PhotoRamki& getInstance() { static PhotoRamki ramki; return ramki; };

		Texture	ramka1Tex, ramka2Tex, ramka3Tex;

		void setup()
		{
			for (int i = 0; i < 3; i++)			
				images[i]   = *AssetManager::getInstance()->getTexture("images/serverScreen/photo"+to_string(i+1)+".png");

			startPosition[0] = Vec2f(53.0f, -537.0f);
			startPosition[1] = Vec2f(733.0f, -455.0f);
			startPosition[2] = Vec2f(1219.0f, -519.0f);

			finalPosition[0] = Vec2f(53.0f, 0.0f);
			finalPosition[1] = Vec2f(733.0f, 0.0f);
			finalPosition[2] = Vec2f(1219.0f, 0.0f);			
		}

		void initAnimationParams()
		{
			for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
			{
				if (PlayerData::playerData[i].isSuccess)
				{
					alphaAnimateComics[i] = 0;
					timeline().apply( &alphaAnimateComics[i], 1.0f, 0.7f, EaseOutCubic() ).delay(0.5f*i);		
				}
				animatePosition[i] = startPosition[i];
				timeline().apply( &animatePosition[i], startPosition[i], finalPosition[i], 0.7f, EaseOutCubic() ).delay(0.5f*i);
			}
			
		}

		void draw()
		{			
			for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
			{		
				gl::pushMatrices();
						gl::translate(animatePosition[i] );									
						gl::draw( images[i]);
				gl::popMatrices();
			}		
			
			/*for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
			{
				if(PlayerData::playerData[i].isSuccess )
				{
					gl::pushMatrices();
						gl::translate(505.f*i, 200.0f );
						gl::scale(0.5f, 0.5f);	
						gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimateComics[i]));					
						gl::draw( PlayerData::getDisplayingTexture(i));
					gl::popMatrices();
				}	
			}*/
			gl::color(Color::white());
		}

		private:
			ci::Anim<float> alphaAnimateComics[3];
			ci::Anim<Vec2f> animatePosition[3];	
			Vec2f startPosition[3], finalPosition[3];
			Texture		images[3];
	
};
inline PhotoRamki&	photoRamki() { return PhotoRamki::getInstance(); };