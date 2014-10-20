#include "PlayerData.h"

PlayerData::OnePlayerPoseData PlayerData::playerData[POSE_IN_GAME_TOTAL];
int				  PlayerData::score;
ci::Surface		  PlayerData::finalImageSurface;

void PlayerData::initData()
{
	score = 0;

	for (int i = 0; i < POSE_IN_GAME_TOTAL; i++)
	{
		playerData[i].isFocusError = false;
		playerData[i].isSuccess = false;
		playerData[i].pathHiRes = "";
		playerData[i].screenshot = ci::Surface(100,600, true);
		playerData[i].storyCode = -1;
	}
}

std::string PlayerData::getTexPath(int i)
{	
	return playerData[i].pathHiRes;
}

ci::gl::Texture PlayerData::getComicsImage(int i)
{	
	return kinect().getPoseImageById(playerData[i].storyCode);
}

void PlayerData::setDisplayingTexture(int i, ci::gl::Texture tex)
{
	playerData[i].displayingTexture = tex;
}

ci::gl::Texture  PlayerData::getDisplayingTexture(int i)
{
	return playerData[i].displayingTexture;
}