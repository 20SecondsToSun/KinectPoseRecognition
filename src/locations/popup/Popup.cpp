#include "Popup.h"

using namespace ci;

void	PopupBase::setup()
{
	closeBtn = new ButtonColor(ci::app::getWindow(), Rectf(1700,100, 1900, 200), Color(1,0,0),
							fonts().getFont("Helvetica Neue", 26),
							"ÇÀÊÐÛÒÜ");	
}

void	PopupBase::reset()
{
	isDrawing = false;	
}

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

void	PopupBase::start(int _type)
{
	type = _type;
	isDrawing  = true;
	screenShot = gl::Texture(ci::app::copyWindowSurface());	
	closeBtnSignal =  closeBtn->mouseDownEvent.connect(boost::bind(&PopupBase::closeHandled, this));

}

void	PopupBase::closeHandled()
{
	cleanPopup();
	closeEvent();
}

void	PopupBase::cleanPopup()
{
	closeBtnSignal.disconnect();
	isDrawing = false;	
}