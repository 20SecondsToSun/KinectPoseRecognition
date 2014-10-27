#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"
#include "cinder/Base64.h"
#include "cinder/Timeline.h"

#include "FontStore.h"
#include "Utils.h"
#include "AssetsManager.h"

class QRcode
{
	public:
		void		setup();
		void		reset();
		void		draw();
		void		initAnim();
		void        drawError();

		bool isReady;		


		bool		isError;
		void		setData(std::string str, std::string link);
private:
		ci::Font						qrCodeFont ;
		ci::Anim<ci::Vec2f>				startQRCodeHolderXY;
		ci::Anim<float>					alphaAnimate;	
		ci::gl::Texture					qrCodeFon, qrCodeTextTexture, qrCodeTexture, *errorTexture;	
	
		std::string				url;
		std::string				stringQrcode;

		gl::Texture				loadImageFromString(std::string value);

		void setTextureString(std::string str);
		void setLink(std::string link);

		gl::Texture	plashkaTex, loadPhotoText;
};