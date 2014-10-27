#include "QRcode.h"

void QRcode::setup()
{
	plashkaTex             = *AssetManager::getInstance()->getTexture("images/serverScreen/plashka.png");
	loadPhotoText          = *AssetManager::getInstance()->getTexture("images/serverScreen/loadPhotoText.png");

	startQRCodeHolderXY = Vec2f(1130.0f, 512.0f);	
}

void QRcode::reset()
{
	isError = false;
	isReady = false;
}

void QRcode::draw()
{
	
	gl::pushMatrices();					
		gl::translate(startQRCodeHolderXY);
		//gl::draw(qrCodeFon);

		if (isError)
		{
			//drawError();			
		}
		else
		{
			if (isReady == false)
			{
				
			}
			else
			{
				if(stringQrcode=="") return;
				
				gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
				gl::draw(loadPhotoText);
	
				gl::translate(Vec2f(-10.0f, 160.0f));				
				gl::draw(plashkaTex);
				
				TextLayout simple;
				simple.setFont( *fonts().getFont("Myriad Pro", 27) );
				simple.setColor(Color::hex(0x197ec7));
				simple.addLine(url);
				qrCodeTextTexture = gl::Texture( simple.render( true, false ) );

				
				if(qrCodeTexture)
				{					
					gl::pushMatrices();			
					gl::translate(74, 36);
					qrCodeTexture = Utils::resizeScreenshot(Surface(qrCodeTexture), (int32_t)208, (int32_t)208);	
					gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
					gl::draw(qrCodeTexture);
					gl::popMatrices();
				}

				if(qrCodeTextTexture)
				{
					gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
					gl::draw(qrCodeTextTexture, Vec2f(0.5f*(plashkaTex.getWidth() - qrCodeTextTexture.getWidth()), 250.0f));	
				}	

			}
		}

	gl::popMatrices();
	gl::color(Color::white());
}

void QRcode::drawError()
{
	gl::pushMatrices();			
			gl::translate(97.0f,650.0f);
			gl::draw(*errorTexture);			
	gl::popMatrices();
}

void QRcode::setTextureString(std::string str)
{
	stringQrcode = str;
	qrCodeTexture = loadImageFromString(stringQrcode);
	
}

void QRcode::setLink(std::string link)
{	
	url =  link;
	url = url.substr(7);
	console()<<"  url   "<<url<<endl;
}

void QRcode::setData(std::string str, std::string link)
{
	setTextureString(str);
	setLink(link);	
	
	isReady = true;
	alphaAnimate = 0.0f;
	timeline().apply( &alphaAnimate, 1.0f, 0.8f, EaseOutCubic() ).delay(0.8f);
}

void QRcode::initAnim()
{
	timeline().apply( &startQRCodeHolderXY,	Vec2f(1270.0f,-971.0f), Vec2f(1270.f, 0.f), 0.6f, EaseOutQuad() );
}

gl::Texture QRcode::loadImageFromString(std::string value)
{	
	Buffer buff = fromBase64(value);
	auto myImage = ci::loadImage(DataSourceBuffer::create(buff), ImageSource::Options(), "png");
	return gl::Texture(myImage);
}