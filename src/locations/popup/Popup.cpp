#include "Popup.h"

using namespace ci;

void	PopupBase::setup()
{
	closeBtn		  = new ButtonColor(Rectf(1700,100, 1900, 200), Color(1,0,0), fonts().getFont("Helvetica Neue", 26),	"ÇÀÊÐÛÒÜ");	
	keyBoardMainBgTex = AssetManager::getInstance()->getTexture(  "keyboard/06_podl.jpg" );	
}

void	PopupBase::reset()
{
	isDrawing = false;	
}

void	PopupBase::draw()
{
	if (isDrawing)
	{
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 0.6f));
		gl::draw(screenShot);
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 0.8f));
		gl::drawSolidRect(ci::app::getWindowBounds());

		gl::color(Color(1.0f, 1.0f, 1.0f));

		gl::pushMatrices();
			gl::translate(bgPosition);
			gl::color(bgColor);
			gl::draw(*keyBoardMainBgTex);				
		gl::popMatrices();

		gl::pushMatrices();
				gl::translate(0.0f, 674.0f);
				gl::translate(bgPosition);
				touchKeyboard().draw();
				closeBtn->draw();	
		gl::popMatrices();
	}
}

void	PopupBase::show(int _type)
{
	type = _type;
	isDrawing  = true;
	screenShot = gl::Texture(ci::app::copyWindowSurface());	

	touchKeyboard().setPosition( Vec2f(360.0f, getWindowHeight() - 504.0f));

	bgPosition =  Vec2f(0.0f, 0.0f);
	bgColor =  ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	timeline().apply( &bgPosition, Vec2f(0.0f,  getWindowHeight() - 1754.0f), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &PopupBase::initHandlers, this ) );
	timeline().apply( &bgColor,  ColorA(1.0f, 1.0f, 1.0f, 0.98f), 0.5f, EaseOutQuart());
}

void PopupBase::initHandlers()
{
	keyboardTouchSignal = touchKeyboard().keyboardTouchSignal.connect(boost::bind(&PopupBase::keyboardTouchSignalHandler, this));
	closeBtnSignal =  closeBtn->mouseDownEvent.connect(boost::bind(&PopupBase::hide, this));
}

void	PopupBase::keyboardTouchSignalHandler()
{

}

void PopupBase::hide()
{
	timeline().apply( &bgPosition,Vec2f(0.0f, 0.0f), 0.6f, EaseInQuart()).finishFn( bind( &PopupBase::closedHandler, this ) );		
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.4f), 0.5f, EaseInQuart()).delay(0.4f);
}

void	PopupBase::closedHandler()
{
	disconnectAll();
	closeEvent();
}

void PopupBase::disconnectAll()
{
	closeBtnSignal.disconnect();
	keyboardTouchSignal.disconnect();
	isDrawing = false;	
}