#include "PlayerData.h"

PlayerData::OnePlayerPoseData PlayerData::playerData[POSE_IN_GAME_TOTAL];
PlayerData::FragmentOptions PlayerData::fragmentOptions;

int PlayerData::score;
int PlayerData::photosWithoutError;
ci::Surface PlayerData::finalImageSurface;
ci::Vec2f PlayerData::finalShift;

void PlayerData::initData()
{
	score = 0;
	photosWithoutError = 0;

	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		playerData[i].isFocusError = false;
		playerData[i].isSuccess = false;
		playerData[i].pathHiRes = "";
		playerData[i].screenshot = ci::Surface(100,600, true);
		playerData[i].storyCode = -1;
		playerData[i].isMask = false;
	}
}

std::string PlayerData::getTexPath(int i)
{	
	return playerData[i].pathHiRes;
}

void PlayerData::setFragmentScaleOptions(int fboWidth, int fboHeight, float poseScale, ci::Vec2f poseShiftVec)
{
	fragmentOptions.fboWidth = fboWidth;
	fragmentOptions.fboHeight = fboHeight;
	fragmentOptions.poseShiftVec = poseShiftVec;
}

int PlayerData::getFragmentWidth()
{
	return fragmentOptions.fboWidth;
}

int PlayerData::getFragmentHeight()
{
	return fragmentOptions.fboHeight;
}

ci::Vec2f PlayerData::getFragmentShiftVec()
{
	return fragmentOptions.poseShiftVec;
}