#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "ofxMSASpline.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

class GameControLScreen
{
	public:	

		static GameControLScreen& getInstance() { static GameControLScreen game; return game; };

		static const int circlesNum = 4;

		Anim<float>	 alphaBubble, silhouetteAlpha, matchingPopupAlpha;
		Anim<Vec2f> timerVec, plashkaVec, krugPercentAnimateVec;

		Texture sidePlashka, krugPercent, sector, ciferblat, arrow, silhouette, percentTexure, matchingPopup;

		Font debugFont26, debugFontText, percentFont;		

		float rotor, quickAnimPosePercent, qaPercent;
		int quickAnimTime, state, showingSeconds, startingTime;
		float  matchingProgress;	
		int percentMatching;

		ofxMSASpline2D spline2D;
		Pose* currentPose;

		vector<Vec2f> animationPointsStates;
		Vec2f animationPosition;

		enum{
			RUNNING,
			FINISHING,
			HIDING,
			STATIC_SHOWING,
			QUICK_ANIMATION
		};

		void setup()
		{
			sidePlashka   =  *AssetManager::getInstance()->getTexture( "images/diz/sidePlashka.png" );
			krugPercent   =  *AssetManager::getInstance()->getTexture( "images/diz/krugPercent.png" );
			sector		  =  *AssetManager::getInstance()->getTexture( "images/diz/sector.png" );
			ciferblat     =  *AssetManager::getInstance()->getTexture( "images/diz/ciferblat.png" );
			arrow         =  *AssetManager::getInstance()->getTexture( "images/diz/arrow.png" );
			matchingPopup =  *AssetManager::getInstance()->getTexture( "images/diz/poseAlreadyBubble.png" );


			timerVec              = Vec2f(-300.0f, 1076.0f);
			plashkaVec            = Vec2f(2100.0f, 0.0f);
			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			silhouetteAlpha		  = 0;

			debugFont26   = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 160);
			percentFont   = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 190);
			debugFontText = Font(loadFile(getAssetPath("fonts/myriad/MyriadPro-BlackSemiExt.ttf")), 26);


			Font percentTexureFont  = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 70);
			percentTexure = Utils::getTextField("%", &percentTexureFont,  Color(1,1,1));			

			spline2D.reserve(4);		
			spline2D.push_back(Vec2f(1794,148));			
			spline2D.push_back(Vec2f(1761,394));			
			spline2D.push_back(Vec2f(1757,650));		
			spline2D.push_back(Vec2f(1793,970));
			spline2D.setInterpolation(OFX_MSA_SPLINE_CUBIC);
			animationPointsStates = spline2D.getPoints(101);
			std::reverse(animationPointsStates.begin(),animationPointsStates.end());  

			setDetentionPercent(0);

			state = HIDING;
		}

		void draw()
		{
			gl::pushMatrices();
				gl::translate(kinect().getTranslation());
				gl::scale(kinect().getScale());	
				gl::color(ColorA(1, 1, 1, silhouetteAlpha));			
				if(currentPose)
					currentPose->draw();
			gl::popMatrices();

			gl::color(Color::white());
			gl::pushMatrices();
				gl::translate(timerVec);
				gl::draw(sector);
			gl::popMatrices();
			
			gl::pushMatrices();
					gl::translate(timerVec);
					gl::translate(Vec2f(0.0, 38.0f+515.0f*0.5f));
					gl::rotate(rotor);
					gl::translate(Vec2f(-515.0f*0.5f, -515.0f*0.5f));							
					gl::draw(ciferblat);					
			gl::popMatrices();	

			gl::pushMatrices();
				gl::translate(timerVec);
				gl::translate(Vec2f(0.0f, 2.0f));
				gl::draw(arrow);

				gl::translate(Vec2f(75.0f, 40.0f));		
				gl::Texture time = Utils::getTextField(to_string(showingSeconds), &debugFont26, Color(1,1,1));

				gl::pushMatrices();
					gl::translate(10 + 0.5*(125 - time.getWidth()), 0);
					gl::draw(time);
				gl::popMatrices();

				gl::color(Color::white());
			gl::popMatrices();
			
			gl::pushMatrices();
				gl::translate(plashkaVec);
				gl::draw(sidePlashka);
			gl::popMatrices();

			gl::pushMatrices();
				gl::translate(krugPercentAnimateVec);
				gl::translate(animationPosition);
				gl::translate(-krugPercent.getWidth()*0.5, -krugPercent.getHeight()*0.5 );
				gl::draw(krugPercent);
				gl::Texture percent = Utils::getTextField(to_string(percentMatching), &percentFont,  Color(1,1,1));

				float totalWidth = percent.getWidth() + percentTexure.getWidth();

				gl::color(Color::hex(0xc42f39));
				gl::translate( 20 + (krugPercent.getWidth() - totalWidth)*0.5, 12 );
				gl::draw(percent);
				gl::translate(percent.getWidth()-36, 28);
				gl::draw(percentTexure);
				gl::color(Color::white());			
			gl::popMatrices();

			//kinect().drawSkeletJoints();			

			if (matchingPopupAlpha > 0.0f)
			{
				gl::pushMatrices();
					gl::color(ColorA(1, 1, 1, matchingPopupAlpha));
					gl::translate(981, 440);
					gl::draw(matchingPopup);
					gl::translate(269, 342);
					int numActiveCircles = circlesNum*matchingProgress;
					for (int i = 0; i < circlesNum; i++)
					{
						gl::pushMatrices();
							gl::translate(20 + 60*i, 0);
							if (i+1<=numActiveCircles)							
								gl::color(ColorA(196.0/255, 47.0/255, 57.0/255, matchingPopupAlpha));
							else
								gl::color(ColorA(223.0/255, 223.0/255, 223.0/255, matchingPopupAlpha));
							gl::drawSolidCircle(Vec2f(0,0), 20, 20);
						gl::popMatrices();
					}
				gl::popMatrices();
			}
			gl::color(Color::white());
		}
	
		void setTimeForAnimation(int seconds)
		{
			showingSeconds = seconds;
		}

		void show()
		{	
			if (state == HIDING)
			{
				silhouetteAlpha = 0.0f;
				state  = STATIC_SHOWING;			
				initStartAnimation();	
			}
		}

		void show1()
		{	
			if (state == HIDING)
			{
				silhouetteAlpha = 0.0f;
				state  = STATIC_SHOWING;			
				initStartAnimation1();	
			}
		}

		void show2()
		{
			rotor = 0;
			timerVec = Vec2f(-300.0f, 1076.0f);
			timeline().apply( &timerVec, Vec2f(0.0f, 776.0f), 1.2f, EaseOutCubic() );
		}		

		void initStartAnimation()
		{
			rotor = 0;
			timerVec = Vec2f(-300.0f, 1076.0f);
			timeline().apply( &timerVec, Vec2f(0.0f, 776.0f), 1.2f, EaseOutCubic() );

			plashkaVec = Vec2f(2100.0f, 0.0f);
			timeline().apply( &plashkaVec, Vec2f(1756.0f , 0.0f), 1.2f, EaseOutCubic() );

			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			timeline().apply( &krugPercentAnimateVec, Vec2f(0.0f , 0.0f), 0.9f, EaseOutCubic() );
		}

		void initStartAnimation1()
		{
			plashkaVec = Vec2f(2100.0f, 0.0f);
			timeline().apply( &plashkaVec, Vec2f(1756.0f , 0.0f), 1.2f, EaseOutCubic() );

			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			timeline().apply( &krugPercentAnimateVec, Vec2f(0.0f , 0.0f), 0.9f, EaseOutCubic() );
		}

		void quickAnimation(int timeToQuickAnimate)
		{
			if (state == QUICK_ANIMATION) return;
			state  = QUICK_ANIMATION;			
		}

		void quickAnimationSetProgress(float percent)
		{
			if(percent > 1) percent = 1;
			if(percent < 0) percent = 0;

			showingSeconds = startingTime + (int)((quickAnimTime - startingTime) * (1-percent));			
			setDetentionPercent(quickAnimPosePercent * (1- percent));

			state = RUNNING;
		}

		void setTime(int seconds)
		{			
			startingTime = seconds;
		}

		void setShowingTime(int seconds)
		{			
			showingSeconds = seconds;
			rotor+=0.8;
		}

		void setQuickAnimTime(int time)
		{
			quickAnimTime = time;
			showingSeconds = time;
		}

		void setQuickAnimPosePercent(float percent)
		{
			quickAnimPosePercent = percent;
			percentMatching = percent;
			setDetentionPercent(percent);
		}		

		void run()
		{
			initStartAnimation();
			start();
		}

		void start()
		{
			state  = RUNNING;
			setDetentionPercent(0);
		}

		void hide()
		{
			state = HIDING;
			timeline().apply( &timerVec, Vec2f(-300.0f, 1076.0f), 1.2f, EaseOutCubic() );
			timeline().apply( &plashkaVec,  Vec2f(2100.0f, 0.0f), 1.2f, EaseOutCubic() );
			timeline().apply( &krugPercentAnimateVec,  Vec2f(300.0f, 0.0f), 1.2f, EaseOutCubic() );
			timeline().apply( &silhouetteAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic() );
			timeline().apply( &matchingPopupAlpha, 0.0f, 0.3f, EaseInCubic() );
		}

		void setDetentionPercent(float percent)
		{
			if(percent > 1) percent = 1;
			if(percent < 0) percent = 0;

			percentMatching = (int)(percent*100);

			if(percentMatching>=100)
				percentMatching -= 1;

			animationPosition = animationPointsStates[percentMatching];			
		}		

		void showSilhouette()
		{			
			silhouetteAlpha = 0.0f;
			timeline().apply( &silhouetteAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic() );
		}

		void setCurrentPose(Pose *pose)
		{
			currentPose = pose;
		}

		void showMatching(float  progressInt)
		{
			if (progressInt <= 0)
			{
				if (matchingPopupAlpha > 0.0f)
				{
					timeline().apply( &matchingPopupAlpha, 0.0f, 0.4f, EaseInCubic() );
				}
			}
			else
			{
				if (matchingPopupAlpha <= 0.0f)
				{
					timeline().apply( &matchingPopupAlpha, 1.0f, 0.4f, EaseInCubic() );
				}
			}

			matchingProgress = progressInt;
		}
};

inline GameControLScreen&	gameControls() { return GameControLScreen::getInstance(); };