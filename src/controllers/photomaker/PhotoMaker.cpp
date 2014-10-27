#include "PhotoMaker.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace  poseParams;
using namespace  photoParams;

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
				string  url = PlayerData::getTexPath(i) ;
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

	if (totalLoaded == PlayerData::score)
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

void PhotoMaker::resizeFinalImages()
{
	mFbo					= gl::Fbo( BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT);

	Vec2f  trans;//
	try
	{
		trans = Vec2f(0.0f, cameraCanon().translateSurface.y);
	}
	catch(...)
	{
		trans = Vec2f(0.0f, 0.0f);
	}	

	int finalImageHeight =  BIG_PHOTO_HEIGHT*PlayerData::score;
	Surface finalImage = Surface(BIG_PHOTO_WIDTH, finalImageHeight, true);

	int offsetI = 0;

	for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			Texture photoFromCameraTex = PlayerData::playerData[i].imageTexture;
			Surface photoFromCameraSurface = Surface(photoFromCameraTex);
		
			Surface cadrSurface = Surface(getWindowWidth(),  getWindowHeight(), true);
			cadrSurface.copyFrom(photoFromCameraSurface, Area(0, 0, getWindowWidth(), getWindowHeight()-trans.y), trans);
			cadrSurface = Utils::resizeScreenshot(cadrSurface, (int32_t)BIG_PHOTO_WIDTH, (int32_t)BIG_PHOTO_HEIGHT);

			drawToFBO(cadrSurface, recognitionGame().getPoseImageById(PlayerData::playerData[i].storyCode));
			
			mFbo.getTexture().setFlipped(true);
			Surface comicsImage = Surface(mFbo.getTexture());

			Surface displaySurface;			
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
			}
			writeImage( Params::getTempPhotoSavePath(i), comicsImage);
			Vec2f offset = Vec2f(0.0f, (float)BIG_PHOTO_HEIGHT*offsetI);
			finalImage.copyFrom(comicsImage, Area(0, 0, BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT), offset);	
			offsetI ++;
		}
	}

	PlayerData::finalImageSurface = finalImage;

	fs::path path = Params::getFinalImageStoragePath();
	writeImage( path, finalImage);
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
	  gl::translate((float)BIG_PHOTO_WIDTH, 0.0f);
	  gl::scale(-1.0f, 1.0f);
	  gl::draw( img );		
	  gl::draw(comicsImage);

	  gl::popMatrices();	
	  gl::setViewport(saveView);
}