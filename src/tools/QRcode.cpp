#include "QRcode.h"
#include "FontStore.h"

void QRcode::setup()
{
	preloader.setup();
}

void QRcode::init()
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
				gl::pushMatrices();
					gl::translate(220, 650);	
					preloader.draw();			
				gl::popMatrices();
			}
			else
			{
				if(stringQrcode=="") return;

				
				//if (isRender == false)
				//{
					isRender = true;		
					qrCodeTexture = loadImageFromString(stringQrcode);

					TextLayout simple;
					simple.setFont( *fonts().getFont("Helvetica Neue", 46) );
					simple.setColor( Color( 1, 1, 1 ) );
					simple.addLine(url);
					qrCodeTextTexture = gl::Texture( simple.render( true, false ) );
				//}
				

				if(qrCodeTextTexture)
				{
					gl::pushMatrices();			
					gl::translate(33, 885);	
					gl::draw(qrCodeTextTexture);
					gl::popMatrices();
				}	

				if(qrCodeTexture)
				{
					gl::pushMatrices();			
					gl::translate(86, 505);		
					gl::draw(qrCodeTexture);
					gl::popMatrices();
				}

			}
		}

	gl::popMatrices();	
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
}

void QRcode::setLink(std::string link)
{	
	url =  link;
	url = url.substr(7);
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