#include "ScreenshotHolder.h"

ci::Surface  ScreenshotHolder::screenshot;
ci::Surface  ScreenshotHolder::screenshotSmall;


void ScreenshotHolder::resize()
{
	/*float w = 1000;
	float h = (w/1920.0)*1080.0;
	int _h = floor(h);

	screenshotSmall = ci::Surface( w, _h , false ); 
	ci::ip::resize( screenshot, screenshot.getBounds(), &screenshotSmall, screenshotSmall.getBounds(), ci::FilterBox() );	*/
}

