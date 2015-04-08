#include "PhotoMaker.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace  poseParams;
using namespace  photoParams;

void PhotoMaker::setup()
{
	mainScale = (float)BIG_PHOTO_WIDTH / getWindowWidth();
	mFbo = gl::Fbo(BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT);
}

void PhotoMaker::loadFinalImages()
{
	startTimer();

	int totalLoaded = 0;

	for (size_t i = 0; i < POSE_IN_GAME_TOTAL; i++)
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
				PlayerData::playerData[i].imageTexture = gl::Texture(PlayerData::playerData[i].screenshot);
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
		if (dirUploadTimer.getSeconds() > photoMakerParams::MAX_WAITING_FROM_DIR_TIME)
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
	Utils::printVideoMemoryInfo();
	bool fboCrashed = false;
	int  offsetI = 0;
	int  finalImageHeight = BIG_PHOTO_HEIGHT * PlayerData::score;

	Surface finalImage = Surface(BIG_PHOTO_WIDTH, finalImageHeight, true);

	for (size_t i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		if (PlayerData::playerData[i].isSuccess)
		{
			Texture photoFromCameraTex = getValidTexture(i);			

			/////////////////////////////////////////////////////////////////
			//
			//		CROP  LOADING PHOTO SURFACE TO FBO
			//
			/////////////////////////////////////////////////////////////////

			try
			{
				//Surface cadrSurface = Surface(loadingPhotoFbo.getTexture());
				int fboWidth = PlayerData::getFragmentWidth();
				int fboHeight = PlayerData::getFragmentHeight();
				Vec2f poseShiftVec = PlayerData::getFragmentShiftVec();

				cropPhotoFbo = gl::Fbo(fboWidth, fboHeight);
				drawCropPhotoToFBO(poseShiftVec, Surface(photoFromCameraTex));
			}
			catch (...)
			{
				fboCrashed = true;
				break;
			}

			// resize crop
			Surface resizedCropPhoto = Surface(cropPhotoFbo.getTexture());
			resizedCropPhoto = Utils::resizeScreenshot(resizedCropPhoto, (int32_t)BIG_PHOTO_WIDTH, (int32_t)BIG_PHOTO_HEIGHT);

			/////////////////////////////////////////////////////////////////
			//
			//		MERGE ALL IN FINALL TO FBO
			//
			/////////////////////////////////////////////////////////////////
			int stroryCode = PlayerData::playerData[i].storyCode;
			try
			{
				drawAllInFinallFBO(resizedCropPhoto, recognitionGame().getPoseImageById(stroryCode), false);
			}
			catch (...)
			{
				fboCrashed = true;
				break;
			}

			/////////////////////////////////////////////////////////////////
			//
			//		WRITE TO FILES
			//
			/////////////////////////////////////////////////////////////////

			Surface comicsImage = Surface(mFbo.getTexture());
			writeImage(Params::getTempPhotoSavePath(i), comicsImage);

			Vec2f offset = Vec2f(0.0f, (float)BIG_PHOTO_HEIGHT * offsetI);
			finalImage.copyFrom(comicsImage, Area(0, 0, BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT), offset);	
			offsetI++;

			try
			{
				Utils::clearFBO(cropPhotoFbo);
			}
			catch (...)
			{
				fboCrashed = true;
				break;
			}
		}
	}

	if (!fboCrashed)
	{
		PlayerData::finalImageSurface = finalImage;
		writeImage(Params::getFinalImageStoragePath(), finalImage);
	}

#ifdef debug
	Utils::printVideoMemoryInfo();
#endif

	return fboCrashed;
}

ci::gl::Texture PhotoMaker::getValidTexture(int index)
{
	if (!PlayerData::playerData[index].isFocusError)
		return PlayerData::playerData[index].imageTexture;
	else
		return PlayerData::playerData[index].screenshot;
}

void PhotoMaker::drawCropPhotoToFBO(Vec2f poseShiftVec, Surface comicsTexture)
{
	Utils::drawGraphicsToFBO(cropPhotoFbo, [&]()
	{
		gl::pushMatrices();
		gl::translate(1920, -100);
		gl::translate(PlayerData::finalShift);
		gl::scale(-1, 1);
		gl::draw(comicsTexture);
		gl::popMatrices();
	});
}

void PhotoMaker::drawAllInFinallFBO(Surface img, ci::gl::Texture comicsImage, bool isMask)
{
	Utils::drawGraphicsToFBO(mFbo, [&]()
	{
		gl::pushMatrices();
		gl::clear(Color::black());
		gl::enableAlphaBlending();
		gl::draw(img);

		gl::scale(mainScale, mainScale);
		gl::draw(comicsImage);
		gl::popMatrices();
	});
}