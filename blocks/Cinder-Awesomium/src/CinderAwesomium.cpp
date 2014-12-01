#include "CinderAwesomium.h"

namespace ph { namespace awesomium {

	// Translates a Cinder virtual key code to an Awesomium key code
	int getWebKeyFromKeyEvent( ci::app::KeyEvent event )
	{
		switch( event.getCode() )
		{
			mapKey(BACKSPACE, BACK)
				mapKey(TAB, TAB)
				mapKey(CLEAR, CLEAR)
				mapKey(RETURN, RETURN)
				mapKey(PAUSE, PAUSE)
				mapKey(ESCAPE, ESCAPE)
				mapKey(SPACE, SPACE)
				mapKey(EXCLAIM, 1)
				mapKey(QUOTEDBL, 2)
				mapKey(HASH, 3)
				mapKey(DOLLAR, 4)
				mapKey(AMPERSAND, 7)
				mapKey(QUOTE, OEM_7)
				mapKey(LEFTPAREN, 9)
				mapKey(RIGHTPAREN, 0)
				mapKey(ASTERISK, 8)
				mapKey(PLUS, OEM_PLUS)
				mapKey(COMMA, OEM_COMMA)
				mapKey(MINUS, OEM_MINUS)
				mapKey(PERIOD, OEM_PERIOD)
				mapKey(SLASH, OEM_2)
				mapKey(0, 0)
				mapKey(1, 1)
				mapKey(2, 2)
				mapKey(3, 3)
				mapKey(4, 4)
				mapKey(5, 5)
				mapKey(6, 6)
				mapKey(7, 7)
				mapKey(8, 8)
				mapKey(9, 9)
				mapKey(COLON, OEM_1)
				mapKey(SEMICOLON, OEM_1)
				mapKey(LESS, OEM_COMMA)
				mapKey(EQUALS, OEM_PLUS)
				mapKey(GREATER, OEM_PERIOD)
				mapKey(QUESTION, OEM_2)
				mapKey(AT, 2)
				mapKey(LEFTBRACKET, OEM_4)
				mapKey(BACKSLASH, OEM_5)
				mapKey(RIGHTBRACKET, OEM_6)
				mapKey(CARET, 6)
				mapKey(UNDERSCORE, OEM_MINUS)
				mapKey(BACKQUOTE, OEM_3)
				mapKey(a, A)
				mapKey(b, B)
				mapKey(c, C)
				mapKey(d, D)
				mapKey(e, E)
				mapKey(f, F)
				mapKey(g, G)
				mapKey(h, H)
				mapKey(i, I)
				mapKey(j, J)
				mapKey(k, K)
				mapKey(l, L)
				mapKey(m, M)
				mapKey(n, N)
				mapKey(o, O)
				mapKey(p, P)
				mapKey(q, Q)
				mapKey(r, R)
				mapKey(s, S)
				mapKey(t, T)
				mapKey(u, U)
				mapKey(v, V)
				mapKey(w, W)
				mapKey(x, X)
				mapKey(y, Y)
				mapKey(z, Z)
				mapKey(DELETE, DELETE)
				mapKey(KP0, NUMPAD0)
				mapKey(KP1, NUMPAD1)
				mapKey(KP2, NUMPAD2)
				mapKey(KP3, NUMPAD3)
				mapKey(KP4, NUMPAD4)
				mapKey(KP5, NUMPAD5)
				mapKey(KP6, NUMPAD6)
				mapKey(KP7, NUMPAD7)
				mapKey(KP8, NUMPAD8)
				mapKey(KP9, NUMPAD9)
				mapKey(KP_PERIOD, DECIMAL)
				mapKey(KP_DIVIDE, DIVIDE)
				mapKey(KP_MULTIPLY, MULTIPLY)
				mapKey(KP_MINUS, SUBTRACT)
				mapKey(KP_PLUS, ADD)
				mapKey(KP_ENTER, SEPARATOR)
				mapKey(KP_EQUALS, UNKNOWN)
				mapKey(UP, UP)
				mapKey(DOWN, DOWN)
				mapKey(RIGHT, RIGHT)
				mapKey(LEFT, LEFT)
				mapKey(INSERT, INSERT)
				mapKey(HOME, HOME)
				mapKey(END, END)
				mapKey(PAGEUP, PRIOR)
				mapKey(PAGEDOWN, NEXT)
				mapKey(F1, F1)
				mapKey(F2, F2)
				mapKey(F3, F3)
				mapKey(F4, F4)
				mapKey(F5, F5)
				mapKey(F6, F6)
				mapKey(F7, F7)
				mapKey(F8, F8)
				mapKey(F9, F9)
				mapKey(F10, F10)
				mapKey(F11, F11)
				mapKey(F12, F12)
				mapKey(F13, F13)
				mapKey(F14, F14)
				mapKey(F15, F15)
				mapKey(NUMLOCK, NUMLOCK)
				mapKey(CAPSLOCK, CAPITAL)
				mapKey(SCROLLOCK, SCROLL)
				mapKey(RSHIFT, RSHIFT)
				mapKey(LSHIFT, LSHIFT)
				mapKey(RCTRL, RCONTROL)
				mapKey(LCTRL, LCONTROL)
				mapKey(RALT, RMENU)
				mapKey(LALT, LMENU)
				mapKey(RMETA, LWIN)
				mapKey(LMETA, RWIN)
				mapKey(LSUPER, LWIN)
				mapKey(RSUPER, RWIN)
				mapKey(MODE, MODECHANGE)
				mapKey(COMPOSE, ACCEPT)
				mapKey(HELP, HELP)
				mapKey(PRINT, SNAPSHOT)
				mapKey(SYSREQ, EXECUTE)
		default: return Awesomium::KeyCodes::AK_UNKNOWN;
		}		
	}

	int getKeyCodeRussian(int letter )
		{
			int code = letter;//(128 +letter)-24;
			switch(letter)
			{
				case 'é':	case 'É':	code = 81;		break;									
				case 'ö':	case 'Ö':	code = 87;		break;
				case 'ó':	case 'Ó':   code = 69;		break;
				case 'ê':	case 'Ê':	code = 82;		break;
				case 'å':	case 'Å':	code = 84;		break;
				case 'í':	case 'Í':	code = 89;		break;
				case 'ã':	case 'Ã':	code = 85;		break;
				case 'ø':	case 'Ø':	code = 73;		break;
				case 'ù':	case 'Ù':	code = 79;		break;
				case 'ç':	case 'Ç':	code = 80;		break;
				case 'õ':	case 'Õ':	code = 219;		break;
				case 'ú':	case 'Ú':	code = 221;		break;
				case 'ô':	case 'Ô':	code = 65;		break;
				case 'û':	case 'Û':	code = 83;		break;
				case 'â':	case 'Â':	code = 68;		break;
				case 'à':	case 'À':	code = 70;		break;
				case 'ï':	case 'Ï':	code = 71;		break;
				case 'ð':	case 'Ð':	code = 72;		break;
				case 'î':	case 'Î':	code = 74;		break;
				case 'ë':	case 'Ë':	code = 75;		break;
				case 'ä':	case 'Ä':	code = 76;		break;
				case 'æ':	case 'Æ':	code = 186;		break;
				case 'ý':	case 'Ý':	code = 222;		break;
				case 'ÿ':	case 'ß':	code = 90;		break;	
				case '÷':	case '×':	code = 88;		break;	
				case 'ñ':	case 'Ñ':	code = 67;		break;	
				case 'ì':	case 'Ì':	code = 86;		break;	
				case 'è':	case 'È':	code = 66;		break;
				case 'ò':	case 'Ò':	code = 78;		break;	
				case 'ü':	case 'Ü':	code = 77;		break;	
				case 'á':	case 'Á':	code = 188;		break;	
				case 'þ':	case 'Þ':	code = 190;		break;				
			}
			return code;
		}

		
	// Conversion functions

	ci::Surface toSurface( Awesomium::BitmapSurface* surface )
	{
		if( ! surface ) throw EmptyPointerException();
		if( ! surface->buffer() ) throw InvalidBufferException();

		// set isDirty to false, because we are manually copying the data
		surface->set_is_dirty( false );

		// create the Surface by copying the data directly 
		return ci::Surface( (uint8_t*) surface->buffer(), surface->width(), surface->height(), surface->row_span(), ci::SurfaceChannelOrder::BGRA );

		// TODO: a more efficient way to render the Surface would be to create a ph::awesomium::Surface class that extends Awesomium::Surface,
		// and use the Paint and Scroll methods to efficiently copy the changed portion. Then use Awesomium::WebCore::set_surface_factory() 
		// to only create Cinder-compatible surfaces.
	}

	ci::gl::Texture toTexture( Awesomium::BitmapSurface* surface, ci::gl::Texture::Format format )
	{
		if( ! surface ) throw EmptyPointerException();
		if( ! surface->buffer() ) throw InvalidBufferException();

		// set isDirty to false, because we are manually copying the data
		surface->set_is_dirty( false );

		// create the gl::Texture by copying the data directly
		return ci::gl::Texture( surface->buffer(), GL_BGRA, surface->width(), surface->height(), format );

		// TODO: a more efficient way to render the Surface would be to create a ph::awesomium::Texture class that extends Awesomium::Surface,
		// and use the Paint and Scroll methods to efficiently copy the changed portion. Then use Awesomium::WebCore::set_surface_factory() 
		// to only create Cinder-compatible surfaces.
	}
	
	Awesomium::WebKeyboardEvent toKeyEvent( ci::app::KeyEvent event, Awesomium::WebKeyboardEvent::Type type, bool isImmitate, char letter )
	{
		//ci::app::console()<<"==========================="<<std::endl;
		//ci::app::console()<<"1 "<<event.getChar()<<std::endl;
		//ci::app::console()<<"2 "<<event.getCode()<<std::endl;
		//ci::app::console()<<"3 "<<event.getNativeKeyCode()<<std::endl;
		//ci::app::console()<<"4 "<<event.getCharUtf32()<<std::endl;

		Awesomium::WebKeyboardEvent evt;
		evt.type = type;
		evt.unmodified_text[0] = event.getChar();

		if(event.getNativeKeyCode() >= 0)
		{
			evt.virtual_key_code = getWebKeyFromKeyEvent( event );
			evt.native_key_code = event.getNativeKeyCode();
			evt.text[0] = event.getChar();   
		}
		else if (isImmitate)
		{
			BYTE kbd[256];
			GetKeyboardState(kbd);
			////GetKeyboardState(pKeyState);
			//BOOL bCapsIsOn = kbd[VK_CAPITAL];
			//ci::app::console()<<"bCapsIsOn  "<<bCapsIsOn<<std::endl;
			//if(!bCapsIsOn) {
			//	keybd_event(VK_CAPITAL, 0x3a, 0, 0);	// these two-line code
			//	keybd_event(VK_CAPITAL, 0x3a, KEYEVENTF_KEYUP, 0);	// will simulate pressing capslock key
			//}

			
			UINT vk = getKeyCodeRussian(letter);//getKeyCodeRussian(letter);//getWebKeyFromKeyEvent( event );   //vitual key represents 'a'
			UINT sc = 0;//event.getNativeKeyCode();    //scan code represents 'a'
			//Just a snippet showing the initialization the buffer part    
			const int BUFFER_LENGTH = 2; //Length of the buffer
			WCHAR chars[BUFFER_LENGTH];

			int res = ToUnicode(vk,sc,kbd,chars,BUFFER_LENGTH,0);
			
			evt.virtual_key_code = vk;
			evt.native_key_code = sc;
			for( int i = 0; i < res; i++) 
				evt.text[i] = chars[i];

			//ci::app::console()<<"0::  "<<chars[0]<<std::endl;
			//ci::app::console()<<"1::  "<<chars[1]<<std::endl;
		}

		char* buf = new char[20];
		Awesomium::GetKeyIdentifierFromVirtualKeyCode(evt.virtual_key_code, &buf);
#if defined( CINDER_MSW )
		strcpy_s<20>(evt.key_identifier, buf);
#else
		strncpy(evt.key_identifier, buf, 20);
#endif
		delete[] buf;

		evt.modifiers = 0;
		if( event.isAltDown() )
			evt.modifiers |= Awesomium::WebKeyboardEvent::kModAltKey;
		if( event.isControlDown() )
			evt.modifiers |= Awesomium::WebKeyboardEvent::kModControlKey;
		if( event.isMetaDown() )
			evt.modifiers |= Awesomium::WebKeyboardEvent::kModMetaKey;
		if( event.isShiftDown() )
			evt.modifiers |= Awesomium::WebKeyboardEvent::kModShiftKey;
		//if( event.isKeypadDown() )	// there is no Cinder isKeypadDown() method at this time
		//	evt.modifiers |= Awesomium::WebKeyboardEvent::kModIsKeypad;

		return evt;
	}

	Awesomium::WebKeyboardEvent toKeyChar( ci::app::KeyEvent event , bool isImmitate, char letter)
	{
		Awesomium::WebKeyboardEvent evt;
		evt.type = Awesomium::WebKeyboardEvent::kTypeChar;
		evt.unmodified_text[0] = event.getChar();

		if(event.getNativeKeyCode() >= 0)
		{
			evt.virtual_key_code = getWebKeyFromKeyEvent( event );
			evt.native_key_code = event.getNativeKeyCode();
			evt.text[0] = event.getChar();   
		}
		else if (isImmitate)
		{
			BYTE kbd[256];
			GetKeyboardState(kbd);
			UINT vk = getKeyCodeRussian(letter);//getWebKeyFromKeyEvent( event );   //vitual key represents 'a'
			UINT sc = 0;//event.getNativeKeyCode();    //scan code represents 'a'
			//Just a snippet showing the initialization the buffer part    
			const int BUFFER_LENGTH = 2; //Length of the buffer
			WCHAR chars[BUFFER_LENGTH];

			int res = ToUnicode(vk,sc,kbd,chars,BUFFER_LENGTH,0);			
			evt.virtual_key_code = vk;
			evt.native_key_code = sc;
			for( int i = 0; i < res; i++) 
				evt.text[i] = chars[i];
		}

	

		return evt;
	} 

} } // namespace ph::awesomium