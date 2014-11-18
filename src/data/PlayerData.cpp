#include "PlayerData.h"

PlayerData::OnePlayerPoseData PlayerData::playerData[POSE_IN_GAME_TOTAL];
int PlayerData::score;
int PlayerData::photosWithoutError;

ci::Surface PlayerData::finalImageSurface;
ci::Vec2f PlayerData::translation;

void PlayerData::setup()
{	
	/*setTranslation(0, Vec2f(36.5f, 128.5f));
	setTranslation(1, Vec2f(23.0f, 144.0f));
	setTranslation(2, Vec2f(40.0f, 86.5f));

	setRotation(0, -2.0f);
	setRotation(1, 0.5f);
	setRotation(2, 1.0f);

	setDefaultTexture(0,  *AssetManager::getInstance()->getTexture( "images/serverScreen/default1.png" ));
	setDefaultTexture(1,  *AssetManager::getInstance()->getTexture( "images/serverScreen/default2.png" ));
	setDefaultTexture(2,  *AssetManager::getInstance()->getTexture( "images/serverScreen/default3.png" ));*/
}

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

void  PlayerData::setDefaultTexture(int i, ci::gl::Texture tex)
{
	playerData[i].defaultTexture = tex;
}

ci::gl::Texture PlayerData::getDefaultTexture(int i)
{
	return playerData[i].defaultTexture;
}