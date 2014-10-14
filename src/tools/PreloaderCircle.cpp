#include "PreloaderCircle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void PreloaderCircle::draw()
{	
	if(preloader)
	{
		gl::pushModelView();	
		gl::scale( 0.5f, 0.5f );
		gl::rotate( 180.0f * float( getElapsedSeconds() ) );
		gl::translate( -0.5f * Vec2f( preloader->getSize() ) );		
		gl::color( Color::white() );
		gl::draw( *preloader );
		gl::popModelView();
	}
}

void PreloaderCircle::setup()
{
	preloader = AssetManager::getInstance()->getTexture("images/preloader.png" );	
}