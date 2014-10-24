#pragma once
#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "AssetsManager.h"

using namespace std;

class PreloaderCircle
{
	public:		
		void draw();	
		void setup();
	private:

		ci::gl::Texture* preloader;			
};