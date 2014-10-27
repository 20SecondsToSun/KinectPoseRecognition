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
			PlayerData::setDisplayingTexture(i, gl::Texture(comicsImage));	
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
      gl::setViewport(mFbo.getBounds() );
	  gl::pushMatrices();
      gl::setMatricesWindow( mFbo.getSize(), false );
      gl::clear( Color( 0, 0, 0 ) );
	  gl::enableAlphaBlending();  
	  gl::translate(BIG_PHOTO_WIDTH, 0 );
	  gl::scale(-1, 1);
	  gl::draw( img );		
	  gl::draw(comicsImage);

	  gl::popMatrices();	
	  gl::setViewport(saveView);
}