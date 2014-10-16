#include "PhotoMaker.h"
#include "AssetsManager.h"
#include "TextureManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace  poseParams;
using namespace  photoParams;

void PhotoMaker::loadFinalImages()
{	
	int totalLoaded = 0;

	for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			if (!PlayerData::playerData[i].isFocusError)
			{
				string url = PlayerData::getTexPath(i) ;
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
	}

}

void PhotoMaker::resizeFinalImages()
{
	mFbo					= gl::Fbo( BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT);

	Vec2f  trans			  = Vec2f(0, cameraCanon().translateSurface.y);

	int finalImageHeight =  BIG_PHOTO_HEIGHT*PlayerData::score;
	Surface finalImage = Surface(BIG_PHOTO_WIDTH, finalImageHeight, true);

	int offsetI = 0;

	for (size_t  i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			Surface surfPhoto = Surface( PlayerData::playerData[i].imageTexture);
		
			Surface cadr = Surface(getWindowWidth(),  getWindowHeight(), true);
			cadr.copyFrom(surfPhoto, Area(0, 0, getWindowWidth(), getWindowHeight()-trans.y), trans);

			cadr = Utils::resizeScreenshot(cadr, (int32_t)BIG_PHOTO_WIDTH, (int32_t)BIG_PHOTO_HEIGHT);
			PlayerData::playerData[i].imageTexture = gl::Texture(cadr);
			drawToFBO(cadr, PlayerData::getComicsImage(i));
			mFbo.getTexture().setFlipped(true);

			PlayerData::playerData[i].imageTexture =  Surface(mFbo.getTexture());

			fs::path path = Params::getTempStorageDirectory() / fs::path( "level"+to_string(i+1) +".jpg");
			Surface comicsImage = Surface(mFbo.getTexture());
			writeImage( path, comicsImage);

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
	  gl::translate(1000, 0 );
	  gl::scale(-1, 1);
	  gl::draw( img );		
	  gl::draw(comicsImage);

	  gl::popMatrices();	
	  gl::setViewport(saveView);
}
