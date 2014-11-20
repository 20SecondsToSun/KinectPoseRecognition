#include "PhotoMaker.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace  poseParams;
using namespace  photoParams;

void PhotoMaker::setup()
{
		maskShader	 = gl::GlslProg(loadAsset("shaders/mask_vert.glsl"), loadAsset("shaders/mask_frag.glsl")); 
}

void PhotoMaker::loadFinalImages()
{	
	startTimer();

	int totalLoaded = 0;

	for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			if (!PlayerData::playerData[i].isFocusError)
			{
				string  url = PlayerData::getTexPath(i);
				Texture tex = ph::fetchTexture(url);

				if (tex)
				{
					PlayerData::playerData[i].imageTexture = tex;
					totalLoaded++;
				}
			}
			else
			{
				PlayerData::playerData[i].imageTexture =  gl::Texture(PlayerData::playerData[i].screenshot);
			}
		}
	}

	if (totalLoaded == PlayerData::photosWithoutError)
	{
		photoLoadEvent();
		stopTimer();
	}
	else
	{
		if(dirUploadTimer.getSeconds() > photoMakerParams::MAX_WAITING_FROM_DIR_TIME)
		{
			photoLoadErrorEvent();
			stopTimer();
		}
	}
}

void PhotoMaker::startTimer()
{
	if (dirUploadTimer.isStopped())
		dirUploadTimer.start();
}

void PhotoMaker::stopTimer()
{
	if (!dirUploadTimer.isStopped())
		dirUploadTimer.stop();
}

int PhotoMaker::getElapsedSeconds()
{
	if (!dirUploadTimer.isStopped())
		return photoMakerParams::MAX_WAITING_FROM_DIR_TIME - (int)dirUploadTimer.getSeconds();
	else 
		return photoMakerParams::MAX_WAITING_FROM_DIR_TIME;
}

bool PhotoMaker::resizeFinalImages()
{
	bool fboCrashed = false;

	mFbo = gl::Fbo( BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT);

	Vec2f  trans;//
	try
	{
		trans = Vec2f( 0, cameraCanon().translateSurface.y);
	}
	catch(...)
	{
		trans = Vec2f(0.0f, 0.0f);
	}	

	console()<<"TRANS::::::::::::::  "<<trans<<endl;

	int finalImageHeight =  BIG_PHOTO_HEIGHT*PlayerData::score;
	Surface finalImage = Surface(BIG_PHOTO_WIDTH, finalImageHeight, true);

	int offsetI = 0;

	for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			Texture photoFromCameraTex;

			if (!PlayerData::playerData[i].isFocusError)
			{
				photoFromCameraTex = PlayerData::playerData[i].imageTexture;
			}
			else
			{
				photoFromCameraTex = PlayerData::playerData[i].screenshot;
			}

			Surface photoFromCameraSurface = Surface(photoFromCameraTex);

			/*Surface cadrSurface = Surface(getWindowWidth(),  getWindowHeight(), true);

			if (!PlayerData::playerData[i].isFocusError)
			{
				cadrSurface.copyFrom(photoFromCameraSurface, Area(0, 0, getWindowWidth(), getWindowHeight()-trans.y), trans);
			}
			else
			{
				cadrSurface = Surface(photoFromCameraTex);
			}*/
			Surface cadrSurface;// = Surface(getWindowWidth(),  getWindowHeight(), true);
			mFboFirst = gl::Fbo(getWindowWidth(),  1080);
			try
			{
				drawToFBOFirstCadr(photoFromCameraSurface);
			}
			catch(...)
			{
				fboCrashed = true;
				break;
			}

			cadrSurface = Surface(mFboFirst.getTexture());

			//writeImage( Params::getTempPhotoSavePath(1), cadrSurface);///!!


			int fboWidth = PlayerData::getFragmentWidth();
			int fboHeight = PlayerData::getFragmentHeight();
			Vec2f poseShiftVec = PlayerData::getFragmentShiftVec();

			mFbo1 = gl::Fbo(fboWidth, fboHeight);
			
			try
			{
				drawToFBOFragmentScale(poseShiftVec, cadrSurface);
			}
			catch(...)
			{
				fboCrashed = true;
				break;
			}

			Surface cadrSurfaceFragment = Surface(mFbo1.getTexture());

			cadrSurfaceFragment = Utils::resizeScreenshot(cadrSurfaceFragment, (int32_t)BIG_PHOTO_WIDTH, (int32_t)BIG_PHOTO_HEIGHT);

			mFboPoseMaskTexture1= gl::Fbo( BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT);
			try
			{				
				drawToFBOposeMaskTexture1(cadrSurfaceFragment);
				finalMask = mFboPoseMaskTexture1.getTexture();
			}
			catch(...)
			{
				fboCrashed = true;
				break;
			}

			try
			{
				drawToFBO(cadrSurfaceFragment, recognitionGame().getPoseImageById(PlayerData::playerData[i].storyCode));
			}
			catch(...)
			{
				fboCrashed = true;
				break;
			}

			Surface comicsImage = Surface(mFbo.getTexture());

			/*Surface displaySurface;

			if (i == 0)
			{
				displaySurface = Utils::resizeScreenshot(comicsImage, (int32_t)560, (int32_t)314);
				PlayerData::setDisplayingTexture(i, gl::Texture(displaySurface));
				PlayerData::setTranslation(i, Vec2f(36.5f, 128.5f));
				PlayerData::setRotation(i, -2.0f);
			}
			else if (i == 1)
			{
				displaySurface = Utils::resizeScreenshot(comicsImage, (int32_t)411, (int32_t)227);
				PlayerData::setDisplayingTexture(i, gl::Texture(displaySurface));
				PlayerData::setTranslation(i, Vec2f(23.0f, 144.0f));
				PlayerData::setRotation(i, 0.5f);
			}
			else if (i == 2)
			{
				displaySurface = Utils::resizeScreenshot(comicsImage, (int32_t)561, (int32_t)315);
				PlayerData::setDisplayingTexture(i, gl::Texture(displaySurface));
				PlayerData::setTranslation(i, Vec2f(40.0f, 86.5f));
				PlayerData::setRotation(i, 1.0f);
			}*/
			writeImage( Params::getTempPhotoSavePath(i), comicsImage);
			Vec2f offset = Vec2f(0.0f, (float)BIG_PHOTO_HEIGHT*offsetI);
			finalImage.copyFrom(comicsImage, Area(0, 0, BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT), offset);	
			offsetI++;
		}
	}

	if (fboCrashed == false) 
	{
		PlayerData::finalImageSurface = finalImage;
		fs::path path = Params::getFinalImageStoragePath();
		writeImage( path, finalImage);
	}

	return fboCrashed;
}

void PhotoMaker::drawToFBO(Surface img, ci::gl::Texture comicsImage)
{
	gl::SaveFramebufferBinding bindingSaver;
	mFbo.bindFramebuffer();
	Area saveView = getViewport();
	gl::setViewport(mFbo.getBounds());

	gl::pushMatrices();
	gl::setMatricesWindow( mFbo.getSize(), false);
	gl::clear( Color::black());
	gl::enableAlphaBlending();  

	gl::draw( img );		
	gl::popMatrices();

	gl::pushMatrices();
	gl::setMatricesWindow( mFbo.getSize(), false);
//	
	/*maskShader.bind();
	maskShader.uniform ( "tex", 0 );
	maskShader.uniform ( "mask", 1 );
		
	finalMask.bind(0);
	PlayerData::kinectTex.bind(1);
	gl::drawSolidRect (finalMask.getBounds());

	PlayerData::kinectTex.unbind();
	finalMask.unbind();
	maskShader.unbind();


	gl::draw(PlayerData::kinectTex);*/
	gl::scale((float)BIG_PHOTO_WIDTH/getWindowWidth(), (float)BIG_PHOTO_WIDTH/getWindowWidth());
	gl::draw(comicsImage);
	gl::popMatrices();

	gl::setViewport(saveView);
}

void PhotoMaker::drawToFBOFragmentScale(Vec2f poseShiftVec, Surface comicsTexture)
{
	gl::SaveFramebufferBinding bindingSaver;
	mFbo1.bindFramebuffer();
	Area saveView = getViewport();
	gl::setViewport(mFbo1.getBounds());

	gl::pushMatrices();
	gl::setMatricesWindow( mFbo1.getSize(), false);
	gl::clear( Color::black());	
		gl::pushMatrices();
			//gl::translate(-poseShiftVec * 3);
			gl::translate(PlayerData::finalShift);
			gl::draw(comicsTexture);
		gl::popMatrices();
	gl::popMatrices();

	gl::setViewport(saveView);
}


void PhotoMaker::drawToFBOposeMaskTexture1(Surface img)
{
	gl::SaveFramebufferBinding bindingSaver;
	mFboPoseMaskTexture1.bindFramebuffer();

	Area saveView = getViewport();
	gl::setViewport(mFboPoseMaskTexture1.getBounds());

	gl::pushMatrices();
		gl::setMatricesWindow( mFboPoseMaskTexture1.getSize(), false);
		gl::clear( Color::black());

		gl::draw(img);
		gl::scale((float)BIG_PHOTO_WIDTH/getWindowWidth(), (float)BIG_PHOTO_WIDTH/getWindowWidth());
		gl::draw(PlayerData::poseMaskTexture);

	gl::popMatrices();

	gl::setViewport(saveView);
}

void PhotoMaker::drawToFBOFirstCadr(Surface photo)
{
	gl::SaveFramebufferBinding bindingSaver;
	mFboFirst.bindFramebuffer();
	Area saveView = getViewport();
	gl::setViewport(mFboFirst.getBounds());

	gl::pushMatrices();
	gl::setMatricesWindow( mFboFirst.getSize(), false);
	gl::clear( Color::black());	
		gl::pushMatrices();
			gl::translate(1920, -100);	
			gl::scale(-1, 1);
			gl::draw(photo);
		gl::popMatrices();
	gl::popMatrices();

	gl::setViewport(saveView);
}