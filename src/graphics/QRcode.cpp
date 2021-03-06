#include "QRcode.h"

void QRcode::setup()
{
	plashkaTex             = *AssetManager::getInstance()->getTexture("images/serverScreen/plashka.png");
	loadPhotoText          = *AssetManager::getInstance()->getTexture("images/serverScreen/loadPhotoText.png");

	startQRCodeHolderXY = Vec2f(1036.0f, 591.0f);	
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
	
	if (isError)
	{
		//drawError();			
	}
	else
	{
		if (isReady == false)
		{
				
		}
		else if(stringQrcode!= "") 
		{				
			gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
			gl::draw(loadPhotoText);
	
			gl::translate(Vec2f(0.0f, 148.0f));				
			gl::draw(plashkaTex);
				
			TextLayout simple;
			simple.setFont( *fonts().getFont("Myriad Pro", 27) );
			simple.setColor(Color::hex(0x197ec7));
			simple.addLine(url);
			qrCodeTextTexture = gl::Texture( simple.render( true, false ) );

				
			if(qrCodeTexture)
			{					
				gl::pushMatrices();			
				gl::translate(0.5f*(plashkaTex.getWidth() - 185.0f), 30.0f);
				qrCodeTexture = Utils::resizeScreenshot(Surface(qrCodeTexture), (int32_t)185, (int32_t)185);	
				gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
				gl::draw(qrCodeTexture);
				gl::popMatrices();
			}

			if(qrCodeTextTexture)
			{
				gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnimate));
				gl::draw(qrCodeTextTexture, Vec2f(0.5f * (plashkaTex.getWidth() - qrCodeTextTexture.getWidth()), 220.0f));	
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
	console()<<"  link   "<<link<<endl;
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
	timeline().apply( &startQRCodeHolderXY,	Vec2f(1284.0f, 0.0f), Vec2f(1284.0f, 109.0f), 0.6f, EaseOutQuad() );
}

gl::Texture QRcode::loadImageFromString(std::string value)
{	
	Buffer buff = fromBase64(value);
	auto myImage = ci::loadImage(DataSourceBuffer::create(buff), ImageSource::Options(), "png");
	return gl::Texture(myImage);
}