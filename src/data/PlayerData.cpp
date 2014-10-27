#include "PlayerData.h"

PlayerData::OnePlayerPoseData PlayerData::playerData[POSE_IN_GAME_TOTAL];
int PlayerData::score;
ci::Surface PlayerData::finalImageSurface;
ci::Vec2f PlayerData::translation;

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

void PlayerData::setDisplayingTexture(int i, ci::gl::Texture tex)
{
	playerData[i].displayingTexture = tex;
}

ci::gl::Texture  PlayerData::getDisplayingTexture(int i)
{
	return playerData[i].displayingTexture;
}

void  PlayerData::setTranslation(int i, ci::Vec2f vec)
{
	playerData[i].translation = vec;
}

ci::Vec2f PlayerData::getTranslation(int i)
{
	return playerData[i].translation;
}

void  PlayerData::setRotation(int i, float _degree)
{
	playerData[i].degree = _degree;
}

float PlayerData::getRotation(int i)
{
	return playerData[i].degree;
}