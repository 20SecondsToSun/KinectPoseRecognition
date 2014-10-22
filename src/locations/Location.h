#ifndef LOCATION_H
#define LOCATION_H

#include "cinder/app/AppNative.h"
#include "LocationEngine.h"
#include "Params.h"

class Location
{
	public:

		virtual void init(LocationEngine* game) = 0;
		virtual void cleanup() = 0;

		virtual void pause()  = 0;
		virtual void resume() = 0;

		virtual void handleEvents() = 0;
		virtual void mouseEvents(int type = 0)  = 0;
		virtual void keyEvents()    = 0;	

		virtual void update() = 0;
		virtual void draw() = 0;

		void changeState(LocationEngine* game, Location* state) 
		{
			game->changeState(state);
		}

		int		getSecondsToComeBack();
		void	comeBackTimerStart();
		void	comeBackTimerStop();		

	protected:

		bool	isComeBackTimerTouchFired();
		bool    isComeBackTimerKinectFired();
		bool	isPeopleInFrame;
	
		ci::Timer	returnTimer;	
};
#endif