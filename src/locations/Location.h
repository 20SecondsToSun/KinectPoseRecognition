#ifndef LOCATION_H
#define LOCATION_H

#include "LocationEngine.h"
#include "cinder/app/AppNative.h"


class Location
{
public:
	virtual void init(LocationEngine* game) = 0;
	virtual void cleanup() = 0;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual void handleEvents() = 0;
	virtual void mouseEvents() = 0;
	virtual void keyEvents() = 0;
	

	virtual void update() = 0;
	virtual void draw() = 0;

	void changeState(LocationEngine* game, Location* state) {
		game->changeState(state);
	}

protected:
	Location() { }
};

#endif