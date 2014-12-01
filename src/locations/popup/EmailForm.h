#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIO.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"

#include "Server.h"
#include "Curl.h"
#include "Params.h"
#include "Utils.h"
#include "AssetsManager.h"
#include "ButtonTexture.h"
#include "VirtualKeyboard.h"
#include "Saver.h"
#include "PlayerData.h"

class EmailForm
{
public:

	void setup( );
	void show();
	void hide();
	void draw();
	void disconnectAll();

	boost::signals2::signal<void(void)> closeEvent, errorSavingToBaseEvent;
	std::vector<std::string> getEmails();
	std::string EmailForm::getEmailsInString();

	static EmailForm& getInstance() { static EmailForm emailF; return emailF; };

private :

	enum closePopupMode {DEFAULT, CLOSE_MAIL, SEND_MAIL, SENDING_READY, EMAIL_ERROR};

	ci::signals::connection	keyboardTouchSignal;
	ci::signals::connection	deleteAllLettersSignal;
	ci::signals::connection	serverSignalLoadingEmailCheck;		
	ci::signals::connection	addEmailSignal;
	ci::signals::connection	closeBtnSignal, MouseUpCon;

	ButtonTex				*closeEmailBtn;
	ButtonTex				*deleteAllLettersBtn;
	ButtonTex				*addEmailBtn;

	void keyboardTouchSignalHandler();		
	void deleteAllLettersHandler();
	void addEmailHandler();
	void closeSignalHandler();

	void drawEmailInput();
	void drawAdditionEmails();
	
	ci::Anim<ci::Vec2f>  bgPosition;
	ci::Anim<float>  topEmailPos, keyBoardPos;
	ci::Anim<ci::ColorA> errorAlpha, bgColor;
	ci::gl::Texture		 *emailLineTex;
	ci::gl::Texture		addEmailTex, deleteAllTex, closeEmailTex,topEmail;
	ci::Font			emailInputFont, emailAddFont;	

	void initHandlers();		
	void closedHandler();

	void sendToEmailHandler();
	bool savePhotoToLocalBase();
	void serverLoadingEmailHandler();
	void setSendingReadyHandlers();

	void addCurrentEmail(std::string _email);

	std::string currentEmail;
	std::vector<std::string> emailVector, codesVec;
	
	int mode;

	ci::gl::Texture *emailErr, *emailOk,  *preloader, *blue_bg, *red_bg;

	void MouseDown( MouseEvent &event);

};
inline EmailForm&	emailPopup() { return EmailForm::getInstance(); };