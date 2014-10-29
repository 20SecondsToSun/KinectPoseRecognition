#include "EmailForm.h"
#include <ctype.h>
#include <boost/algorithm/string.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mndl::curl;

void EmailForm::setup()
{	
	keyBoardMainBgTex = AssetManager::getInstance()->getTexture(  "keyboard/06_podl.jpg" );
	emailLineTex = AssetManager::getInstance()->getTexture( "keyboard/emailLine.png"  );
	closeEmailTex = gl::Texture( loadImage( loadAsset("keyboard/closeEmail.png"  )));
	deleteAllTex  = gl::Texture( loadImage(getAssetPath("keyboard/deleteAll.png"  )));
	addEmailTex   = gl::Texture( loadImage( loadAsset( "keyboard/addEmail.png"  )));	


	emailOk   = AssetManager::getInstance()->getTexture("images/social/email_ok.png" );
	emailErr  = AssetManager::getInstance()->getTexture("images/social/email_err.png" );	
	preloader = AssetManager::getInstance()->getTexture("images/social/preloader.png" );
	blue_bg   = AssetManager::getInstance()->getTexture("images/social/blue_bg.png" );
	red_bg    = AssetManager::getInstance()->getTexture("images/social/red_bg.png" );
	

	emailInputFont			= *fonts().getFont("Myriad Pro", 70);
	emailAddFont			= *fonts().getFont("Myriad Pro", 26);

	addEmailBtn = new ButtonTex(addEmailTex, "addEmail");
	addEmailBtn->setScreenField(Vec2f(389.0f, 260.0f));	
	addEmailBtn->setDownState(addEmailTex);

	deleteAllLettersBtn = new ButtonTex(deleteAllTex, "deleteAll");	
	deleteAllLettersBtn->setScreenField(Vec2f(1463.0f, 135.0f));	
	deleteAllLettersBtn->setDownState(deleteAllTex); 

	closeEmailBtn= new ButtonTex(closeEmailTex, "closeEmail");
	closeEmailBtn->setScreenField( Vec2f(1778.0f, 88.0f));	
	closeEmailBtn->setDownState(closeEmailTex);
}

void EmailForm::show()
{
	mode = DEFAULT;

	currentEmail = "";
	emailVector.clear();

	touchKeyboard().setPosition( Vec2f(360.0f, getWindowHeight() - 504.0f));

	bgPosition =  Vec2f(0.0f, 0.0f);
	timeline().apply( &bgPosition, Vec2f(0.0f, 0.0f), Vec2f(0.0f,  getWindowHeight() - 1754.0f), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &EmailForm::initHandlers, this ) );
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.0f), ColorA(1.0f, 1.0f, 1.0f, 0.98f), 0.5f, EaseOutQuart());
}

void EmailForm::initHandlers()
{
	keyboardTouchSignal = touchKeyboard().keyboardTouchSignal.connect(boost::bind(&EmailForm::keyboardTouchSignalHandler, this));
	closeBtnSignal =  closeEmailBtn->mouseDownEvent.connect(boost::bind(&EmailForm::closeSignalHandler, this));
	deleteAllLettersSignal = deleteAllLettersBtn->mouseDownEvent.connect(boost::bind(&EmailForm::deleteAllLettersHandler, this));
	addEmailSignal		=  addEmailBtn->mouseDownEvent.connect(boost::bind(&EmailForm::addEmailHandler, this));
	
}

void EmailForm::MouseDown( MouseEvent &event)
{
	console()<<" mode ::  "<<mode<<endl;
	if (mode == SENDING_READY)
	{
		hide();
		return;
	}
	else if (mode == EMAIL_ERROR)
	{
		mode = DEFAULT;
		initHandlers();
	}
}

void EmailForm::closeSignalHandler()
{
	disconnectAll();
	mode = CLOSE_MAIL;
	hide();
}

void EmailForm::keyboardTouchSignalHandler()
{
	string lastCode = touchKeyboard().getLastCode();		

	if (touchKeyboard().isBackCode())
	{
		currentEmail = currentEmail.substr(0, currentEmail.size() - 1);	
		return;
	}

	if (currentEmail.size() > 30)   return;

	if (touchKeyboard().isMailCode())
	{
		if (currentEmail.size() == 0) return;	
				
		if(currentEmail[currentEmail.size() - 1] =='@')
		{
			currentEmail = currentEmail + lastCode;
			boost::to_upper(currentEmail);
		}
		else
		{
			currentEmail = currentEmail + "@" + lastCode;
			boost::to_upper(currentEmail);
		}
		return;
	}
	else if (touchKeyboard().isSendCode())
	{
		if ((currentEmail.size() == 0 && emailVector.size() == 0 ) ||(currentEmail.size() && !Utils::isValidEmail(currentEmail)))
		{	
			mode = EMAIL_ERROR;
			disconnectAll();
			MouseUpCon	   = getWindow()->getSignalMouseDown().connect(   std::bind( &EmailForm::MouseDown,   this, std::placeholders::_1 ) );
			
			return;
		}
		if (currentEmail.size() != 0)
			addCurrentEmail(currentEmail);			
		
		sendToEmailHandler();
		return;
	}

	currentEmail = currentEmail + lastCode;
	boost::to_upper(currentEmail);
}


void EmailForm::sendToEmailHandler()
{
	mode = SEND_MAIL;
	disconnectAll();

	console()<<"TRY TO SAVE PHOTOS.......... "<< server().isPhotoLoaded <<endl;
	if (server().isPhotoLoaded)
	{
		serverSignalLoadingEmailCheck = server().sendToMailEvent.connect( 
			boost::bind(&EmailForm::serverLoadingEmailHandler, this) 
		);
		server().sendToMail(getEmailsInString());
	}
	else
	{
		savePhotoToLocalBase();
		mode = SENDING_READY;
		MouseUpCon	   = getWindow()->getSignalMouseDown().connect(   std::bind( &EmailForm::MouseDown,   this, std::placeholders::_1 ) );
	}
}

void EmailForm::serverLoadingEmailHandler()
{
	server().stopTimeout();

	if (server().isEmailSent == false)
	{	
		savePhotoToLocalBase();
	}
	else
	{
		console()<<"SERVER SAVE MAILS!!!!! "<<endl;
	}

	mode = SENDING_READY;
	MouseUpCon	   = getWindow()->getSignalMouseDown().connect(   std::bind( &EmailForm::MouseDown,   this, std::placeholders::_1 ) );
}

void EmailForm::savePhotoToLocalBase()
{
	console()<<"SERVER ERROR. SAVE LOCALY "<<emailPopup().getEmailsInString()<<endl;
	bool status = saver().saveImageIntoBase(emailPopup().getEmailsInString(), PlayerData::finalImageSurface);
}


std::vector<std::string> EmailForm::getEmails()
{
	return emailVector;
}

std::string EmailForm::getEmailsInString()
{
	string allEmails = "";
	for (size_t i = 0; i < emailVector.size(); i++)
	{
		if ( i != emailVector.size()-1)
		{
			allEmails +=emailVector[i] +",";
		}
		else allEmails +=emailVector[i];		
	}

	return allEmails;
}

void EmailForm::disconnectAll()
{
	keyboardTouchSignal.disconnect();
	closeBtnSignal.disconnect();
	deleteAllLettersSignal.disconnect();
	addEmailSignal.disconnect();
	serverSignalLoadingEmailCheck.disconnect();
	MouseUpCon.disconnect();
}

void EmailForm::hide()
{
	disconnectAll();
	timeline().apply( &bgPosition,Vec2f(0.0f, 0.0f), 0.6f, EaseInQuart()).finishFn( bind( &EmailForm::closedHandler, this ) );		
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.1f), 0.5f, EaseInQuart()).delay(0.4f);
}

void EmailForm::closedHandler()
{	
	closeEvent();
}

void EmailForm::deleteAllLettersHandler()
{
	currentEmail = "";	
}

void EmailForm::addEmailHandler()
{
	if (currentEmail.size() == 0)
	{
		
	}
	else if (Utils::isValidEmail(currentEmail) == false)
	{
		mode = EMAIL_ERROR;
		disconnectAll();
		MouseUpCon	   = getWindow()->getSignalMouseDown().connect(   std::bind( &EmailForm::MouseDown,   this, std::placeholders::_1 ) );
		
	}
	else
	{
		addCurrentEmail(currentEmail);
	}	
}

void EmailForm::drawEmailInput()
{
	TextLayout _text;
	_text.setFont( emailInputFont );
	_text.setColor( Color::white());	
	_text.addLine(currentEmail);	
	 gl::Texture textTexture = gl::Texture( _text.render( true, false ) );

	 Vec2f _scale = Vec2f(1.0f,1.0f);
	
	 if (textTexture.getWidth() > 1050.0f)
	{
		float sc = 1050.0f / textTexture.getWidth();
		_scale= Vec2f(sc, sc);		
	}

	float coordy = deleteAllLettersBtn->getY() - _scale.x*textTexture.getHeight()*0.5f;

	gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
	gl::pushMatrices();		
		gl::translate(Vec2f(391.0f, coordy));
		gl::scale(_scale);
		gl::draw(textTexture);
		gl::translate(Vec2f(textTexture.getWidth(),0.0f));
		if ((int)getElapsedSeconds() % 2 == 0 )
		gl::drawSolidRect(Rectf(0.0f, 0.0f, 8.0f, textTexture.getHeight()));
	gl::popMatrices();
}

void EmailForm::drawAdditionEmails()
{
	if (emailVector.size() == 0 ) return;

	for (size_t i = 0, ilen = emailVector.size(); i < ilen; i++)
	{
		TextLayout simple;
		simple.setFont(emailAddFont);
		simple.setColor(Color::white());
		simple.addLine(emailVector[i]);	

		gl::Texture textTexture = gl::Texture( simple.render( true, false ) );

		gl::color(Color::white());

		gl::pushMatrices();
				gl::translate(Vec2f(393.0f + 1108.0f -textTexture.getWidth() ,246.0f + i *(textTexture.getHeight() + 3.0f)));				
				gl::draw(textTexture);
		gl::popMatrices();
	}
}

void EmailForm::addCurrentEmail(string _email)
{
	if (emailVector.size() >= 3 ) 
		return;

	emailVector.push_back(_email);
	currentEmail = "";	
}

void EmailForm::draw()
{
	gl::color(Color(1.0f, 1.0f, 1.0f));
	
	if (mode == SEND_MAIL)
	{
		gl::color(bgColor);
		gl::draw(*blue_bg);
		gl::Texture text= Utils::getTextField("���������� �����...",  fonts().getFont("Myriad Pro", 70), Color::white());
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
		return;
	}

	if (mode == SENDING_READY)
	{
		gl::color(bgColor);
		gl::draw(*blue_bg);
		gl::draw(*emailOk, Vec2f((getWindowWidth() - 711.0f)*0.5f, (getWindowHeight() - 460.0f)*0.5f ));	
		return;
	}

	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::color(bgColor);
		gl::draw(*keyBoardMainBgTex);
		gl::translate(0.0f, 1754.0f - 1080.0f);
		drawEmailInput();
		drawAdditionEmails();
	gl::popMatrices();

	gl::pushMatrices();
			gl::translate(0.0f, 674.0f);
			gl::translate(bgPosition);
			touchKeyboard().draw();
	gl::popMatrices();	

	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::translate(Vec2f(0.0f, 674.0f));
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
		addEmailBtn->draw();
		
		gl::color(Color::white());
		gl::pushMatrices();
				gl::translate(Vec2f(393.0f, 206.0f));
				gl::draw(*emailLineTex);
		gl::popMatrices();

		gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));

		if (currentEmail != "")
			deleteAllLettersBtn->draw();

		closeEmailBtn->draw();
	gl::popMatrices();	


	if (mode == EMAIL_ERROR)
	{
		gl::color(bgColor);
		gl::draw(*red_bg);
		gl::draw(*emailErr, Vec2f((getWindowWidth() - 711.0f)*0.5f, (getWindowHeight() - 460.0f)*0.5f ));		
	}
}