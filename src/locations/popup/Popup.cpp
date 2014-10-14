#include "Popup.h"

using namespace ci;

void	PopupBase::draw()
{
	if (isDrawing)
	{
		gl::color(ColorA(1,1,1,0.6));
		gl::draw(screenShot);
		gl::color(ColorA(1, 1, 1,0.8));
		gl::drawSolidRect(ci::app::getWindowBounds());

		closeBtn->draw();
	}
}

void	PopupBase::start()
{
	screenShot = gl::Texture(ci::app::copyWindowSurface());
	isDrawing  = true;
	closeBtnSignal =  closeBtn->mouseDownEvent.connect(boost::bind(&PopupBase::closeHandled, this));	
}

void	PopupBase::setup()
{
	closeBtn = new ButtonColor(ci::app::getWindow(), Rectf(1700,100, 1900, 200), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 26),
							"«¿ –€“‹");	
}

void	PopupBase::closeHandled()
{
	closeBtnSignal.disconnect();
	isDrawing = false;
	closeEvent();
}