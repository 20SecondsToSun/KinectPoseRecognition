#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "AssetsManager.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"
#include "PreloaderCircle.h"
#include "cinder/Base64.h"

class QRcode
{
	public:
		void		setup();
		void		init();
		void		draw();
		void		initAnim();
		void        drawError();

		bool isReady, isRender;		
		void setTextureString(std::string str);
		void setLink(std::string link);


		bool		isError;
private:
		ci::Font						qrCodeFont ;
		ci::Anim<ci::Vec2f>				startQRCodeHolderXY;
		ci::gl::Texture					qrCodeFon, qrCodeTextTexture, qrCodeTexture, *errorTexture;	

		PreloaderCircle					preloader;

		std::string				url;
		std::string				stringQrcode;		

		gl::Texture				loadImageFromString(std::string value);
};