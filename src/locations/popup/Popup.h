#pragma once

#include "cinder/Cinder.h"
#include "cinder/DataSource.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "ButtonColor.h"
#include "FontStore.h"

#include <boost/thread.hpp>
#include <boost/functional/hash.hpp>

	
class PopupBase
{
public:	
	// singleton implementation
	static PopupBase& getInstance() { static PopupBase tm; return tm; };


	void			draw();
	std::string		type;
	void			start();
	void			setup();
	bool			isDrawing;

	ci::gl::Texture		screenShot;

	boost::signals2::signal<void(void )> closeEvent;
	ci::signals::connection	closeBtnSignal;

	ButtonColor			*closeBtn;

private:
	void	closeHandled();


};

// helper function(s) for easier access 
inline PopupBase&	popup() { return PopupBase::getInstance(); };