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

#include <boost/thread.hpp>
#include <boost/functional/hash.hpp>
#include "VirtualKeyboard.h"
#include "AssetsManager.h"

	
class PopupBase
{
	public:	
		// singleton implementation
		static PopupBase& getInstance() { static PopupBase popup; return popup; };

		void			setup();
		void			draw();
		void			reset();	
		void			show(int popuptype);
		void			disconnectAll();	
		bool			isDrawing;

		ci::gl::Texture		screenShot;

		boost::signals2::signal<void(void )> closeEvent;
		ci::signals::connection	closeBtnSignal, keyboardTouchSignal;

		ButtonColor			*closeBtn;

	private:		

		void	closedHandler();	
		void	initHandlers();
		void	keyboardTouchSignalHandler();
		void	hide();
		int		type;

		ci::Anim<ci::Vec2f>  bgPosition;
		ci::Anim<ci::ColorA> bgColor;
		ci::gl::Texture		*keyBoardMainBgTex;	
};

inline PopupBase&	socialPopup() { return PopupBase::getInstance(); };