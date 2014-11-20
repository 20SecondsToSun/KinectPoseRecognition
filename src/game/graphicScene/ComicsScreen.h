#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "CameraAdapter.h"
#include "cinder/Text.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

class ComicsScreen
{
	public:

		static ComicsScreen& getInstance() { static ComicsScreen game; return game; };
	
		bool isGuess;
		Texture comicsTexture, poseTexture, failImage, bg_blue, poseMaskTexture;
		Texture kinectHumanMaskTexture, kinectTex, finalMask, kinectTex1;
		Anim<float> alphaAnim;

		float poseScale, lastScale;
		Vec2f poseShiftVec, middleVec, finalShift, finalShift1;

		Fbo mFbo, mFboKinect, mFboCanon, mFboErasedMask, mFboPoseMaskTexture1, mFboKinect1;
		bool fboCrashed;

		gl::GlslProg maskShader;

		boost::signals2::signal<void(void)> errorFBO;

		void setup()
		{
			failImage    = *AssetManager::getInstance()->getTexture( "images/fail.jpg" );
			bg_blue		 = *AssetManager::getInstance()->getTexture( "images/diz/bg.jpg" );
			maskShader	 = gl::GlslProg(loadAsset("shaders/mask_vert.glsl"), loadAsset("shaders/mask_frag.glsl")); 
			kinectHumanMaskTexture		 = *AssetManager::getInstance()->getTexture( "images/mask.png" );

			alphaAnim = 0;
			isGuess = false;
		}

		bool show()
		{
			float x1 = middleVec.x * 3 + kinect().viewShiftX;// * poseScale;
			float y1 = middleVec.y * 3 + kinect().viewShiftY;// * poseScale;

			float x2 = x1*poseScale;
			float y2 = y1*poseScale;

			float finX = x1 - x2;
			float finY = y1 - y2;
			finalShift = -Vec2f(finX, finY);
			PlayerData::finalShift = finalShift;


			

			

			lastScale = 1 - (poseScale - (1000.0f/getWindowWidth()));
			lastScale > 1 ? lastScale = 1: lastScale = lastScale;
			lastScale < 0 ? lastScale = 0.1: lastScale = lastScale;

			float x3 = x1*lastScale;
			float y3 = y1*lastScale;

		

			float finX1 = x1 - x3;
			float finY1 = y1 - y3;
			finalShift1 = -Vec2f(finX1, finY1);
			PlayerData::finalShift1 = finalShift1;




			int fboWidth =  (int)(1920.0f * poseScale);
			int fboHeight =  (int)(1080.0f * poseScale);
			mFbo = gl::Fbo(fboWidth, fboHeight);
			mFboKinect = gl::Fbo(1920, 1080);
			mFboCanon = gl::Fbo(1920, 1080);
			mFboErasedMask = gl::Fbo(1920, 1080);	
			mFboPoseMaskTexture1= gl::Fbo(1920, 1080);	
			mFboKinect1= gl::Fbo(1920, 1080);	

			fboCrashed = false;			
			try
			{
				drawToFBOKinectMask();
				kinectTex = mFboKinect.getTexture();
				console()<<"kinectTex :::  "<<kinectTex.getWidth()<<":::  "<<kinectTex.getHeight()<<endl;
			}
			catch(...)
			{
				fboCrashed = true;			
			}

			try
			{
				drawToFBOKinectMask1();
				kinectTex1 = mFboKinect1.getTexture();
				console()<<"kinectTex :::  "<<kinectTex1.getWidth()<<":::  "<<kinectTex1.getHeight()<<endl;
			}
			catch(...)
			{
				fboCrashed = true;			
			}	

			



			try
			{
				drawToFBOCanonSurface();
			}
			catch(...)
			{
				fboCrashed = true;			
			}

			try
			{
				drawToFBOposeMaskTexture1();
				finalMask = mFboPoseMaskTexture1.getTexture();
			}
			catch(...)
			{
				fboCrashed = true;			
			}

			
				
			try
			{
				drawToFBO();
			}
			catch(...)
			{
				fboCrashed = true;			
			}	

			if(fboCrashed)
			{
				errorFBO();
				return false;
			}
			

			PlayerData::setFragmentScaleOptions(fboWidth, fboHeight, poseScale, poseShiftVec);

			PlayerData::kinectTex = kinectTex1;
			PlayerData::poseMaskTexture = poseMaskTexture;

			Params::saveBufferSuccessComics(mFbo.getTexture());

			return true;
		}

		void draw()
		{
			if (isGuess)
			{
				drawPoseComics();
			}
			else
			{
				gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnim));
				gl::draw(failImage);
				gl::color(Color::white());
			}
		}

		void setPoseScale(float  _poseScale)
		{
			poseScale = _poseScale;
		}

		void setPoseShift(Vec2f  _poseShiftVec)
		{
			poseShiftVec = _poseShiftVec;
		}
		void setMiddlePoint(Vec2f  _poseShiftVec)
		{
			middleVec = _poseShiftVec;
		}
		

	

		void drawToFBO()
		{
			gl::SaveFramebufferBinding bindingSaver;
			mFbo.bindFramebuffer();

			Area saveView = getViewport();
			gl::setViewport(mFbo.getBounds());

			gl::enableAlphaBlending();

			gl::pushMatrices();
				gl::setMatricesWindow( mFbo.getSize(), false);
				gl::clear( Color::black());

				gl::draw(mFboCanon.getTexture());

			/*	maskShader.bind();
				maskShader.uniform ( "tex", 0 );
				maskShader.uniform ( "mask", 1 );
				
				finalMask.bind(0);
				kinectTex.bind(1);
				gl::drawSolidRect (finalMask.getBounds());

				kinectTex.unbind();
				finalMask.unbind();
				maskShader.unbind();
*/



				
			
				/*maskShader.bind();
				maskShader.uniform ( "tex", 0 );
				maskShader.uniform ( "mask", 1 );
		
				poseMaskTexture.bind(0);
				kinectTex.bind(1);
				gl::drawSolidRect (poseMaskTexture.getBounds());

				kinectTex.unbind();
				poseMaskTexture.unbind();
				maskShader.unbind();*/

			/*	gl::pushMatrices();
					gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
					gl::scale(kinect().headScale, kinect().headScale);
					gl::draw(kinectHumanMaskTexture);
				gl::popMatrices();	*/
				gl::scale(poseScale, poseScale);
				

				gl::draw(poseTexture);
			gl::popMatrices();

			gl::setViewport(saveView);
		}

		void drawToFBOKinectMask()
		{
			gl::SaveFramebufferBinding bindingSaver;
			mFboKinect.bindFramebuffer();

			Area saveView = getViewport();
			gl::setViewport(mFboKinect.getBounds());

			gl::pushMatrices();
			gl::setMatricesWindow( mFboKinect.getSize(), false);
			gl::clear( Color(0,0,1));

			gl::enableAlphaBlending(); 
				gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
				gl::translate(finalShift);
				gl::scale(kinect().headScale, kinect().headScale);
				gl::draw(kinectHumanMaskTexture);
			gl::popMatrices();	

			gl::setViewport(saveView);
		}

		void drawToFBOKinectMask1()
		{
			gl::SaveFramebufferBinding bindingSaver;
			mFboKinect1.bindFramebuffer();

			Area saveView = getViewport();
			gl::setViewport(mFboKinect1.getBounds());

			gl::pushMatrices();
			gl::setMatricesWindow( mFboKinect1.getSize(), false);
			gl::clear( Color(0,0,1));

			gl::enableAlphaBlending(); 
				gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
				gl::translate(finalShift1);
				gl::scale(lastScale, lastScale);
				gl::scale(kinect().headScale, kinect().headScale);
				gl::draw(kinectHumanMaskTexture);
			gl::popMatrices();	

			gl::setViewport(saveView);
		}














		void drawToFBOCanonSurface()
		{
			gl::SaveFramebufferBinding bindingSaver;
			mFboCanon.bindFramebuffer();

			Area saveView = getViewport();
			gl::setViewport(mFboCanon.getBounds());

			gl::pushMatrices();
			gl::setMatricesWindow( mFboCanon.getSize(), false);
			gl::clear( Color::black());

			gl::enableAlphaBlending(); 
				
			gl::translate(cameraCanon().getSurfaceTranslate());
			gl::translate(finalShift);
			gl::scale(-1.8181818, 1.8181818);
			gl::draw(comicsTexture);

			gl::popMatrices();

			gl::setViewport(saveView);
		}

		void drawToFBOposeMaskTexture1()
		{
			gl::SaveFramebufferBinding bindingSaver;
			mFboPoseMaskTexture1.bindFramebuffer();

			Area saveView = getViewport();
			gl::setViewport(mFboPoseMaskTexture1.getBounds());

			gl::pushMatrices();
				gl::setMatricesWindow( mFboPoseMaskTexture1.getSize(), false);
				gl::clear( Color::black());

				gl::draw(mFboCanon.getTexture());
				gl::scale(poseScale, poseScale);
				gl::draw(poseMaskTexture);

			gl::popMatrices();

			gl::setViewport(saveView);
		}




		void drawPoseComics()
		{
			gl::color(ColorA(1.0f, 1.0f, 1.0f, alphaAnim));
				gl::draw(bg_blue);
			gl::color(Color::white());
			
			/*gl::pushMatrices();
				gl::translate(cameraCanon().getSurfaceTranslate());
				gl::scale(-cameraCanon().scaleFactor, cameraCanon().scaleFactor);
				gl::draw(comicsTexture);
			gl::popMatrices();*/

			gl::pushMatrices();
				gl::translate(poseShiftVec * 3);
				gl::draw(mFbo.getTexture());
				//gl::scale(poseScale, poseScale);
				//gl::draw(comicsTexture);
				//gl::draw(poseTexture);
				//gl::translate(poseShiftVec);
			gl::popMatrices();

			//float x1 = poseShiftVec.x * 3;// * poseScale;
			//float y1 = poseShiftVec.y * 3;// * poseScale;

			//gl::drawStrokedRect(Rectf(Area(x1, y1, x1 + 1920.0f * poseScale, y1 + 1080.0f * poseScale)));
		}
};

inline ComicsScreen&	comicsScreen() { return ComicsScreen::getInstance(); };