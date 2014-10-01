#include "PlayerData.h"

ci::Surface PlayerData::screenshot[3];
bool		PlayerData::isSuccess[3];
int			PlayerData::score;

void PlayerData::initData()
{
	score = 0;

	for (int i = 0; i < 3; i++)
	{
		isSuccess[i] = false;
	}

}