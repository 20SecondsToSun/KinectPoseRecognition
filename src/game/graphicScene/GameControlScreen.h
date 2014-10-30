#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

class GameControLScreen
{
	public:	

		static GameControLScreen& getInstance() { static GameControLScreen game; return game; };

		static const int circlesNum = 3;

		Anim<float>	 alphaBubble, silhouetteAlpha, matchingPopupAlpha;
		Anim<Vec2f> timerVec, plashkaVec, krugPercentAnimateVec;

		Texture sidePlashka, krugPercent, sector, ciferblat;
		Texture	arrow, silhouette, percentTexure, matchingPopup, poseTestTex, palka;

		Font secondsFont, debugFontText, percentFont;		

		float rotor, quickAnimPosePercent, qaPercent;
		int quickAnimTime, state, showingSeconds, startingTime;
		float  matchingProgress;	
		int percentMatching;

	
		Pose* currentPose;

		Vec2f animationPosition;

		float degree, degree1;

		enum{
			RUNNING,
			FINISHING,
			HIDING,
			STATIC_SHOWING,
			QUICK_ANIMATION
		};

		void setup()
		{
			sidePlashka   =  *AssetManager::getInstance()->getTexture( "images/diz/sidePlashka.png");
			krugPercent   =  *AssetManager::getInstance()->getTexture( "images/diz/krugPercent.png");
			sector		  =  *AssetManager::getInstance()->getTexture( "images/diz/sector.png");
			ciferblat     =  *AssetManager::getInstance()->getTexture( "images/diz/ciferblat.png");
			arrow         =  *AssetManager::getInstance()->getTexture( "images/diz/arrow.png");
			matchingPopup =  *AssetManager::getInstance()->getTexture( "images/diz/poseAlreadyBubble.png");

			poseTestTex   =  *AssetManager::getInstance()->getTexture( "images/Cat0.png");
			palka         =  *AssetManager::getInstance()->getTexture( "images/diz/palka.png");

			timerVec              = Vec2f(-300.0f, 1076.0f);
			plashkaVec            = Vec2f(2100.0f, 0.0f);
			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			silhouetteAlpha = 0;

			secondsFont   = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 170);
			percentFont   = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 190);
			debugFontText = Font(loadFile(getAssetPath("fonts/MyriadPro-BlackSemiExt.ttf")), 26);

			Font percentTexureFont  = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 70);
			percentTexure = Utils::getTextField("%", &percentTexureFont,  Color(1,1,1));			

		

			setDetentionPercent(0);

			state = HIDING;
		}

		void draw()
		{
			gl::pushMatrices();
				gl::translate(Vec2f(0.0f, (1080.0f-1440.0f)*0.5));
				gl::color(ColorA(1, 1, 1, silhouetteAlpha));
				if(currentPose)
					currentPose->draw();
			gl::popMatrices();

			gl::color(Color::white());
			gl::draw(sector, timerVec);
			
			gl::pushMatrices();
					gl::translate(timerVec);
					gl::translate(Vec2f(0.0f, 38.0f+515.0f*0.5f));
					gl::rotate(rotor);
					gl::translate(Vec2f(-515.0f*0.5f, -515.0f*0.5f));
					gl::draw(ciferblat);
			gl::popMatrices();

			gl::pushMatrices();
				gl::translate(timerVec);
				gl::translate(Vec2f(0.0f, 2.0f));
				gl::draw(arrow);

				gl::translate(Vec2f(75.0f, 40.0f));
				gl::Texture time = Utils::getTextField(to_string(showingSeconds), &secondsFont, Color(1,1,1));
				gl::draw(time, Vec2f(10.0f + 0.5f*(125 - time.getWidth()), 0.0f));
				gl::color(Color::white());
			gl::popMatrices();
		
			gl::draw(sidePlashka, plashkaVec);		

			gl::color(Color::white());	
			float radius = 4237*0.5;
			float shiftX = 1915.0f;
			float theta  = -sin(degree)/cos(degree);
			float angle  = 90+57.2957795f*atan(theta);
			if (theta > 0)	angle -=180;			
			
			gl::pushMatrices();				
				gl::translate(radius + shiftX, 1080*0.5);
				gl::translate(-1756.0f, 0);
				gl::translate(plashkaVec);

				gl::translate(radius*sin(degree), radius*cos(degree) );				
				gl::rotate(angle);		
				gl::translate(-palka.getWidth()*0.5, -palka.getHeight()*0.5 );				
				gl::draw(palka);
			gl::popMatrices();

			radius = 4237*0.5;

			gl::pushMatrices();
				gl::translate(krugPercentAnimateVec);
				gl::translate(radius + shiftX - 170, 1080*0.5);
				gl::translate(radius*sin(degree1), radius*cos(degree1));				
				gl::color(Color::white());
				gl::translate(-krugPercent.getWidth()*0.5, -krugPercent.getHeight()*0.5 );
				gl::draw(krugPercent);	

				gl::Texture percent = Utils::getTextField(to_string(percentMatching), &percentFont,  Color(1,1,1));
				float totalWidth = percent.getWidth() + percentTexure.getWidth();
				gl::color(Color::hex(0xc42f39));
				gl::translate(20 + (krugPercent.getWidth() - totalWidth) * 0.5f, 0.0f );
				gl::draw(percent);
				gl::translate(percent.getWidth() - 36.0f, 35.0f);
				gl::draw(percentTexure);
				gl::color(Color::white());		
			gl::popMatrices();
		
			////////////////////////////////////////////
			////////////////////////////////////////////
			////////////////////////////////////////////

			if (matchingPopupAlpha > 0.0f)
			{
				gl::pushMatrices();
					gl::color(ColorA(1.0f, 1.0f, 1.0f, matchingPopupAlpha));
					gl::translate(1186.0f, 0.0f);
					gl::draw(matchingPopup);
					gl::translate(304.0f - 13.0f, 368.0f);
					int numActiveCircles = circlesNum*matchingProgress;
					for (int i = 0; i < circlesNum; i++)
					{
						gl::pushMatrices();
							
							if (i+1<=numActiveCircles)
							{
								gl::translate(13.0f + 68.0f*i, 0.0f);
								gl::color(ColorA(196.0f/255.0f, 47.0f/255.0f, 57.0f/255.0f, matchingPopupAlpha));
								gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 24.0f, 20.0f);
							}
							else
							{
								gl::translate(13.0f + 68.0f*i, 0.0f);
								gl::color(ColorA(223.0f/255.0f, 223.0f/255.0f, 223.0f/255.0f, matchingPopupAlpha));
								gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 13.0f, 20.0f);
							}
							
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

			degree  = 5.16f + (4.71f - 5.16f) * percent;
			degree1 = 4.9095f + (4.52949f - 4.9095f) * percent;
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
				matchingPopupAlpha  = 0.0f;
				//if (matchingPopupAlpha > 0.0f)
				//{
				//	timeline().apply( &matchingPopupAlpha, 0.0f, 0.4f, EaseInCubic() );
				//}
			}
			else
			{
				matchingPopupAlpha = 1.0f;
				//if (matchingPopupAlpha <= 0.0f)
				//{
				//	timeline().apply( &matchingPopupAlpha, 1.0f, 0.4f, EaseInCubic() );
				//}
			}

			matchingProgress = progressInt;
		}
};

inline GameControLScreen&	gameControls() { return GameControLScreen::getInstance(); };