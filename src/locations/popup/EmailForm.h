#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIO.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"

#include "Popup.h"
#include "Curl.h"
#include "Params.h"
#include "Utils.h"
#include "AssetsManager.h"
#include "ButtonTexture.h"
#include "VirtualKeyboard.h"

class EmailForm
{
	public:
		
		void setup( );
		void show();
		void hide();
		void draw();
		void update();
		void disconnectAll();

		boost::signals2::signal<void(void)> closeEvent, sendEvent;
		std::vector<std::string> getEmails();
		std::string EmailForm::getEmailsInString();
			

		// singleton implementation
		static EmailForm& getInstance() { static EmailForm emailF; return emailF; };

	private :

		ci::signals::connection	keyboardTouchSignal;
		ci::signals::connection	deleteAllLettersSignal;
		ci::signals::connection	addEmailSignal;
		ci::signals::connection	closeBtnSignal;

		ButtonTex				*closeEmailBtn;
		ButtonTex				*deleteAllLettersBtn;
		ButtonTex				*addEmailBtn;

		void keyboardTouchSignalHandler();		
		void deleteAllLettersHandler();
		void addEmailHandler();
		void closeSignalHandler();

		void drawEmailInput();
		void drawAdditionEmails();

		VirtualKeyboard		touchKeyBoard;



		ci::Anim<ci::Vec2f>  bgPosition;
		ci::Anim<ci::ColorA> errorAlpha, bgColor;
		ci::gl::Texture		*keyBoardMainBgTex, *emailLineTex;
		ci::gl::Texture		addEmailTex, deleteAllTex, closeEmailTex;
		ci::Font			emailInputFont, emailAddFont;

	
		
		void initHandlers();		
		void closedHandler();

		
		void addCurrentEmail(std::string _email);

		std::string currentEmail;
		std::vector<std::string> emailVector;

		std::string mode;



};
inline EmailForm&	emailPopup() { return EmailForm::getInstance(); };