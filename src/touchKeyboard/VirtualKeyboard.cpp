#include "VirtualKeyboard.h"
#include "AssetsManager.h"
#include "FontStore.h"
#include <ctype.h>

using namespace ci;
using namespace ci::app;
using namespace std;

string VirtualKeyboard::secondLineCharacters[10] = {"q","w","e","r","t","y","u","i","o","p"};
string VirtualKeyboard::thirdLineCharacters[10]  = {"a","s","d","f","g","h","j","k","l","-"};
string VirtualKeyboard::fourthLineCharacters[9]  = {"z","x","c","v","b","n","m",".","_"};

string VirtualKeyboard::secondLineCharacters2[10] = {"[","]","{","}","#","%","^","*","+","="};
string VirtualKeyboard::thirdLineCharacters2[10]  = {"$","&","\\","|","~","<",">","(",")","-"};
string VirtualKeyboard::fourthLineCharacters2[10]  = {"/",":",";",",","?","!","'","\"",".","_"};

string VirtualKeyboard::secondLineCharacters3[11] = {"�","�","�","�","�","�","�","�","�","�","�"};
string VirtualKeyboard::thirdLineCharacters3[11]  = {"�","�","�","�","�","�","�","�","�","�", "�"};
string VirtualKeyboard::fourthLineCharacters3[10]  = {"�","�","�","�","�","�","�","�","�", "�"};

int    VirtualKeyboard::lineLength1 = 10;

float  VirtualKeyboard::_xOffset1 = 14.0f;
float  VirtualKeyboard::_xOffset2 = 14.0f;
float  VirtualKeyboard::_xOffset3 = 13.0f;
float  VirtualKeyboard::_xOffset4 = 12.0f;
float  VirtualKeyboard::_xOffset5 = 12.0f;

ci::Vec2f  VirtualKeyboard::lineOffset1 = Vec2f(360.0f, 30.0f);
ci::Vec2f  VirtualKeyboard::lineOffset2 = Vec2f(415.0f, 122.0f);
ci::Vec2f  VirtualKeyboard::lineOffset3 = Vec2f(455.0f, 214.0f);
ci::Vec2f  VirtualKeyboard::lineOffset4 = Vec2f(510.0f, 306.0f);
ci::Vec2f  VirtualKeyboard::lineOffset5 = Vec2f(504.0f, 398.0f);

void VirtualKeyboard::setup(Vec2f _position)
{
	Font *mFont								= fonts().getFont("Helvetica Neue Light", 32);

	gl::Texture backspaceBtnTex				= gl::Texture( loadImage( loadAsset(   "keyboard/backBtn.png" )));
	gl::Texture _simple						= gl::Texture( loadImage( loadAsset(   "keyboard/_simpleBtn.png" )));	
	gl::Texture	yaBtnTex					= gl::Texture( loadImage( loadAsset(   "keyboard/ya.png")));
	gl::Texture ramBtnTex					= gl::Texture( loadImage( loadAsset(   "keyboard/ram.png")));
	gl::Texture mailBtnTex					= gl::Texture( loadImage( loadAsset(   "keyboard/mail.png" )));
	gl::Texture gmailBtnTex					= gl::Texture( loadImage( loadAsset(   "keyboard/gmail.png" )));

	gl::Texture sendBtnTex					= gl::Texture( loadImage( loadAsset(   "keyboard/send.png" )));
	shiftTex1								= gl::Texture( loadImage( loadAsset(   "keyboard/shift.png" )));
	shiftTex0								= gl::Texture( loadImage( loadAsset(   "keyboard/shift0.png")));

	gl::Texture spaceBtnTex					= gl::Texture( loadImage( loadAsset(   "keyboard/prob.png" )));

	changeKeyboardTex1						= gl::Texture( loadImage( loadAsset(   "keyboard/k2.png" )));
	changeKeyboardTex2						= gl::Texture( loadImage( loadAsset(   "keyboard/k2.png")));

	gl::Texture langChangeTex				= gl::Texture( loadImage( loadAsset("keyboard/lang.png")));

	Vec2f shift_Y= Vec2f::zero();
	float _width = 86.0f;

	

	lineOffset1 = Vec2f(0.0f, 0.0f);
	lineOffset2 = Vec2f(65.0f, 92.0f);
	lineOffset3 = Vec2f(105.0f, 2*92.0f);
	lineOffset4 = Vec2f(150.0f, 3*92.0f);
	lineOffset5 = Vec2f(194.0f, 4*92.0f);
	
	for (auto i = 0; i < lineLength1; i++)
	{
		ButtonTex* btn = new ButtonTex(_simple, mFont, to_string(i));
		btn->setScreenField(lineOffset1 + Vec2f(i*(_xOffset1 + _width), 0.0f)+shift_Y);		
		buttonsMainKeyboard.push_back( btn );
		buttonsSecondKeyboard.push_back( btn );
		buttonsRusMainKeyboard.push_back( btn );
	}
	
	shift = new ButtonTex(shiftTex0, "shift");
	shift->setScreenField(lineOffset4 - Vec2f(100.0f, 0.0f) + shift_Y);
	buttonsMainKeyboard.push_back(shift);
	buttonsRusMainKeyboard.push_back(shift);

	ButtonTex * backspaceBtn = new ButtonTex(backspaceBtnTex, "back");
	backspaceBtn->setScreenField(lineOffset1 + Vec2f(10.0f*(_xOffset1 + _width),0.0f)+shift_Y);	
	buttonsMainKeyboard.push_back(backspaceBtn);
	buttonsSecondKeyboard.push_back(backspaceBtn);
	buttonsRusMainKeyboard.push_back(backspaceBtn);

	

	for (size_t i = 0, ilen = secondLineCharacters->size(); i < 10; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, secondLineCharacters[i]);
		btn->setScreenField(lineOffset2 + Vec2f(i*(_xOffset2 + _width), 0.0f) + shift_Y);		
		buttonsMainKeyboard.push_back( btn );
	}

	for (size_t i = 0, ilen = secondLineCharacters2->size(); i < 10; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, secondLineCharacters2[i]);
		btn->setScreenField(lineOffset2 + Vec2f(i*(_xOffset2 + _width), 0.0f) + shift_Y);		
		buttonsSecondKeyboard.push_back( btn );
	}

	for (size_t i = 0, ilen = secondLineCharacters3->size(); i < 11; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, secondLineCharacters3[i]);
		btn->setScreenField(lineOffset2 + Vec2f(i*(_xOffset2 + _width), 0.0f) + shift_Y);		
		buttonsRusMainKeyboard.push_back( btn );
	}

	for (size_t i = 0, ilen = thirdLineCharacters->size(); i < 10; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, thirdLineCharacters[i]);
		btn->setScreenField(lineOffset3 + Vec2f(i*(_xOffset3 + _width), 0.0f) + shift_Y);		
		buttonsMainKeyboard.push_back( btn );
	}

	for (size_t i = 0, ilen = thirdLineCharacters2->size(); i < 10; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, thirdLineCharacters2[i]);
		btn->setScreenField(lineOffset3 + Vec2f(i*(_xOffset3 + _width), 0.0f) + shift_Y);		
		buttonsSecondKeyboard.push_back( btn );
	}
	
	for (size_t i = 0, ilen = thirdLineCharacters3->size(); i < 11; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, thirdLineCharacters3[i]);
		btn->setScreenField(lineOffset3 + Vec2f(i*(_xOffset2 + _width), 0.0f) + shift_Y);		
		buttonsRusMainKeyboard.push_back( btn );
	}


	for (size_t i = 0, ilen = fourthLineCharacters->size(); i < 9; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, fourthLineCharacters[i]);
		btn->setScreenField(lineOffset4 + Vec2f(i*(_xOffset4 + _width), 0.0f)+ shift_Y);		
		buttonsMainKeyboard.push_back( btn );
	}

	for (size_t i = 0, ilen = fourthLineCharacters2->size(); i < 10; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, fourthLineCharacters2[i]);
		btn->setScreenField(lineOffset4 + Vec2f((i)*(_xOffset4 + _width), 0.0f)+ shift_Y - Vec2f(_xOffset4 + _width, 0.0f));		
		buttonsSecondKeyboard.push_back( btn );
	}

	for (size_t i = 0, ilen = fourthLineCharacters3->size(); i < 10; i++)
	{
		ButtonTex * btn = new ButtonTex(_simple, mFont, fourthLineCharacters3[i]);
		btn->setScreenField(lineOffset4 + Vec2f(i*(_xOffset2 + _width), 0.0f) + shift_Y);		
		buttonsRusMainKeyboard.push_back( btn );
	}




	ButtonTex *dogBtn , *spaceBtn , *sendBtn , *mailBtn, *gmailBtn, *yaBtn, *ramBtn, *langBtn;

	changeKeyboardBtn = new ButtonTex(changeKeyboardTex1, mFont, "#+=");
	changeKeyboardBtn->setScreenField(lineOffset5 + Vec2f(0.0f*(_xOffset5 + _width) - 170.0f, 0.0f)+ shift_Y);	
	buttonsMainKeyboard.push_back(changeKeyboardBtn);
	buttonsSecondKeyboard.push_back(changeKeyboardBtn);
	buttonsRusMainKeyboard.push_back(changeKeyboardBtn);

	langBtn = new ButtonTex(langChangeTex, "lang");
	langBtn->setScreenField(lineOffset5 + Vec2f(0.0f*(_xOffset5 + _width), 0.0f)+ shift_Y);	
	buttonsMainKeyboard.push_back(langBtn);
	buttonsSecondKeyboard.push_back(langBtn);
	buttonsRusMainKeyboard.push_back(langBtn);

	dogBtn = new ButtonTex(_simple, mFont, "@");
	dogBtn->setScreenField(lineOffset5 + Vec2f(1.0f*(_xOffset5 + _width), 0.0f)+ shift_Y);	
	buttonsMainKeyboard.push_back(dogBtn);
	buttonsSecondKeyboard.push_back(dogBtn);
	buttonsRusMainKeyboard.push_back(dogBtn);

	spaceBtn= new ButtonTex(spaceBtnTex, " ");
	spaceBtn->setScreenField(lineOffset5 + 2.0f*Vec2f((_xOffset5 + _width), 0.0f)+ shift_Y);	
	buttonsMainKeyboard.push_back(spaceBtn);
	buttonsSecondKeyboard.push_back(spaceBtn);
	buttonsRusMainKeyboard.push_back(spaceBtn);

	sendBtn= new ButtonTex(sendBtnTex, "send");
	Vec2f pos = lineOffset5 + 2.0f*Vec2f((_xOffset5  + _width), 0.0f) + shift_Y;
	sendBtn->setScreenField(pos+ Vec2f(_xOffset5 + spaceBtnTex.getWidth(), 0.0f));	
	buttonsMainKeyboard.push_back(sendBtn);
	buttonsSecondKeyboard.push_back(sendBtn);
	buttonsRusMainKeyboard.push_back(sendBtn);

	mailBtn= new ButtonTex(mailBtnTex, "mail.ru");
	pos = Vec2f(1295.0f, 30)+shift_Y;
	mailBtn->setScreenField(pos);	
	buttonsMainKeyboard.push_back(mailBtn);
	buttonsSecondKeyboard.push_back(mailBtn);
	buttonsRusMainKeyboard.push_back(mailBtn);

	gmailBtn= new ButtonTex(gmailBtnTex, "gmail.com");
	pos = Vec2f(1295.0f, 122.0f) + shift_Y;
	gmailBtn->setScreenField(pos);	
	buttonsMainKeyboard.push_back(gmailBtn);
	buttonsSecondKeyboard.push_back(gmailBtn);
	buttonsRusMainKeyboard.push_back(gmailBtn);

	yaBtn= new ButtonTex(yaBtnTex, "yandex.ru");
	pos = Vec2f(1295.0f, 214.0f) + shift_Y;
	yaBtn->setScreenField(pos);	
	buttonsMainKeyboard.push_back(yaBtn);
	buttonsSecondKeyboard.push_back(yaBtn);
	buttonsRusMainKeyboard.push_back(yaBtn);

	ramBtn= new ButtonTex(ramBtnTex, "rambler.ru");
	pos = Vec2f(1295.0f, 306.0f) + shift_Y;
	ramBtn->setScreenField(pos);	
	buttonsMainKeyboard.push_back(ramBtn);
	buttonsSecondKeyboard.push_back(ramBtn);
	buttonsRusMainKeyboard.push_back(ramBtn);

	mainWindow = getWindow();
	position = _position;
	show();

	alwaysCaps = false;

	setlocale(LC_ALL, ""); 
}

void VirtualKeyboard::show()
{
	isShiftDown = false;
	isKeyBoardChangeDown = false;
	lastCode = "NONE";
	activeLanguage = KEYBOARD_LANG::ENG;
	activeKeyboard = &buttonsMainKeyboard;
}

void VirtualKeyboard::removeHandlers()
{
	MouseUpCon.disconnect( );
	MouseDownCon.disconnect();
	KeyDownCon.disconnect();
}

void VirtualKeyboard::closeKeyboard()
{
	keyboardClosedSignal();
}

void VirtualKeyboard::initKeyboard()
{	
	MouseDownCon   = mainWindow->getSignalMouseDown().connect(std::bind( &VirtualKeyboard::MouseDown, this, std::placeholders::_1));
	MouseUpCon	   = mainWindow->getSignalMouseUp().connect(std::bind( &VirtualKeyboard::MouseUp, this, std::placeholders::_1));
	KeyDownCon	   =  mainWindow->getSignalKeyDown().connect(std::bind( &VirtualKeyboard::KeyDown, this, std::placeholders::_1));	

	keyboardReadySignal();
}

void VirtualKeyboard::draw( )
{
	gl::pushMatrices();	
	gl::translate(position);
	for( auto item = activeKeyboard->begin(); item != activeKeyboard->end(); ++item )
	{
		(*item)->draw();
	}	
	gl::popMatrices();
}

void VirtualKeyboard::KeyDown( KeyEvent event  )
{

}

void VirtualKeyboard::MouseUp( MouseEvent &event )
{
	for( auto item = activeKeyboard->begin(); item != activeKeyboard->end(); ++item )
	{
		(*item)->up();
	}	
}

void VirtualKeyboard::setPosition(Vec2f _vec )
{
	position = _vec;
}

void VirtualKeyboard::alwaysCapsLock(bool value )
{
	alwaysCaps = value;
}

void VirtualKeyboard::MouseDown( MouseEvent &event )
{
	lastCode ="-1";

	Vec2f coords   = event.getPos() - position;

	for( auto item = activeKeyboard->begin(); item != activeKeyboard->end(); ++item )
	{		
		if((*item)->contains(coords))
		{
			checkCapsLock();
			lastCode = (*item)->getBtnId();
			(*item)->down();
			if (lastCode == "shift")
			{
				changeShiftMode();
				keyboardTouchSignal();
			}
			else if (lastCode == "#+=" || lastCode == "ABC")
			{
				changeKeyboardMode();
				
			}
			else if (lastCode == "lang")
			{				
				changeLangMode();
			}
			else
			{				
				keyboardTouchSignal();
			}

			break;
		}
	}
}

string VirtualKeyboard::getLastCode()
{
	return  lastCode;
}

string VirtualKeyboard::getDisplayCode()
{
	if(alwaysCaps && lastCode.size() == 1)
	{
		char letter = lastCode[0];
		letter = toupper(letter);
		lastCode[0] = letter;
	}
	
	return  Utils::cp1251_to_utf8(lastCode.c_str());
}

void VirtualKeyboard::changeShiftMode()
{
	isShiftDown = !isShiftDown;
	shift->texture = isShiftDown ? shiftTex1 : shiftTex0;	

	for( auto item = activeKeyboard->begin(); item != activeKeyboard->end(); ++item )
	{			
		if((*item)->getBtnId().size()!=1) continue;
		char letter = (*item)->getBtnId()[0];
		if (isalpha((unsigned char)letter) )
		{		
			string oneChar = "";
			if (isShiftDown)			
				oneChar.append(1, toupper(letter));				
			else
				oneChar.append(1, tolower(letter));	

			(*item)->setBtnId(oneChar);
		}
	}
	checkCapsLock();
}

void VirtualKeyboard::changeLangMode()
{
	switch (activeLanguage)
	{
	case KEYBOARD_LANG::ENG:
		activeLanguage = KEYBOARD_LANG::RUS;
		activeKeyboard = &buttonsRusMainKeyboard;
		break;

	case KEYBOARD_LANG::RUS:
		activeLanguage = KEYBOARD_LANG::ENG;
		activeKeyboard = &buttonsMainKeyboard;
		break;

	default:
		break;
	}
	for( auto item = activeKeyboard->begin(); item != activeKeyboard->end(); ++item )
	{			
		if((*item)->getBtnId().size()!=1) continue;
		char letter = (*item)->getBtnId()[0];
		if (isalpha((unsigned char)letter) )
		{		
			string oneChar = "";
			if (isShiftDown)			
				oneChar.append(1, toupper(letter));				
			else
				oneChar.append(1, tolower(letter));	

			(*item)->setBtnId(oneChar);
		}
	}

}

void VirtualKeyboard::changeKeyboardMode()
{
	isKeyBoardChangeDown  = !isKeyBoardChangeDown;

	if (isKeyBoardChangeDown)
	{		
		changeKeyboardBtn->setBtnId("ABC");		
		activeKeyboard = &buttonsSecondKeyboard;		
	}
	else
	{
		changeKeyboardBtn->setBtnId("#+=");

		switch (activeLanguage)
		{
		case KEYBOARD_LANG::ENG:
			activeKeyboard = &buttonsMainKeyboard;
			break;

		case KEYBOARD_LANG::RUS:
			activeKeyboard = &buttonsRusMainKeyboard;
			break;

		default:
			break;
		}
	}
}

KeyEvent VirtualKeyboard::imitate_ENTER_KEY_EVENT()
{
	KeyEvent key(getWindow(), 13,13, 13, 0,13);
	return key;
}

KeyEvent VirtualKeyboard::imitate_BACKSPACE_KEY_EVENT()
{
	KeyEvent key(getWindow(),8,8,ci::app::KeyEvent::KEY_BACKSPACE, 0,8);
	return key;
}

MouseEvent VirtualKeyboard::inititateMouseEvent(Vec2f _vec)
{
	unsigned int k = 1;
	float r = 1.0f;
	uint32_t t = 1;
	return MouseEvent(getWindow(), 1, _vec.x  , _vec.y , k,  r,  t);
}

bool VirtualKeyboard::isMailCode()
{
	return lastCode == "gmail.com" || lastCode == "mail.ru" || lastCode == "yandex.ru" || lastCode == "rambler.ru";
}

bool VirtualKeyboard::isBackCode()
{
	return lastCode == "back";
}

bool VirtualKeyboard::isSendCode()
{
	return lastCode == "send";
}

bool VirtualKeyboard::isShiftCode()
{
	return lastCode == "shift";
}

void VirtualKeyboard::checkCapsLock()
{
	BYTE kbd[256];
	GetKeyboardState(kbd);	
	BOOL bCapsIsOn = kbd[VK_CAPITAL];
	//console()<<"alwaysCaps  "<<alwaysCaps<<"  "<<bCapsIsOn<<endl;
	if(bCapsIsOn != isShiftDown )//|| alwaysCaps !=bCapsIsOn)
	{
		keybd_event(VK_CAPITAL, 0x3a, 0, 0);	// these two-line code
		keybd_event(VK_CAPITAL, 0x3a, KEYEVENTF_KEYUP, 0);	// will simulate pressing capslock key
	}
}

void VirtualKeyboard::onCapsLock()
{
	BYTE kbd[256];
	GetKeyboardState(kbd);	
	BOOL bCapsIsOn = kbd[VK_CAPITAL];
	if(!bCapsIsOn)
	{
		keybd_event(VK_CAPITAL, 0x3a, 0, 0);	// these two-line code
		keybd_event(VK_CAPITAL, 0x3a, KEYEVENTF_KEYUP, 0);	// will simulate pressing capslock key
	}
}

void VirtualKeyboard::offCapsLock()
{
	BYTE kbd[256];
	GetKeyboardState(kbd);	
	BOOL bCapsIsOn = kbd[VK_CAPITAL];
	if(bCapsIsOn)
	{
		keybd_event(VK_CAPITAL, 0x3a, 0, 0);	// these two-line code
		keybd_event(VK_CAPITAL, 0x3a, KEYEVENTF_KEYUP, 0);	// will simulate pressing capslock key
	}
}
