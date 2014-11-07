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

		Anim<float>	 alphaBubble, silhouetteAlpha, matchingPopupAlpha;
		Anim<Vec2f> timerVec, plashkaVec, krugPercentAnimateVec;

		Texture sidePlashka, krugPercent, sector, ciferblat, timer_bubble, popka;
		Texture	arrow, silhouette, percentTexure, matchingPopup, poseTestTex, palka;

		Font secondsFont, debugFontText, percentFont;		

		float rotor, quickAnimPosePercent, qaPercent;		
		float matchingProgress, palkaPosition;
		
		int percentMatching, quickAnimTime, state, showingSeconds, startingTime;

		Pose* currentPose;
		Vec2f animationPosition;

		static const int radius = 2118;
		static const int shiftX = 1915;

		static const int circlesNum = 3;

		enum{
			RUNNING,
			FINISHING,
			HIDING,
			STATIC_SHOWING,
			QUICK_ANIMATION
		};

		void setup()
		{
			sidePlashka   =  *AssetManager::getInstance()->getTexture( "images/diz/sidePlashka.jpg");
			//krugPercent   =  *AssetManager::getInstance()->getTexture( "images/diz/krugPercent.png");
			//sector		  =  *AssetManager::getInstance()->getTexture( "images/diz/sector.png");
			//ciferblat     =  *AssetManager::getInstance()->getTexture( "images/diz/ciferblat.png");
			//arrow         =  *AssetManager::getInstance()->getTexture( "images/diz/arrow.png");
			matchingPopup =  *AssetManager::getInstance()->getTexture( "images/diz/poseAlreadyBubble.png");

			poseTestTex   =  *AssetManager::getInstance()->getTexture( "images/Cat0.png");
			palka         =  *AssetManager::getInstance()->getTexture( "images/diz/kolbasa.png");

			timer_bubble   =  *AssetManager::getInstance()->getTexture( "images/diz/time_bubble.png");
			popka  =  *AssetManager::getInstance()->getTexture( "images/diz/popka.jpg");

			timerVec              = Vec2f(1458.0f, 1080.0f);
			plashkaVec            = Vec2f(1920.0f, 0.0f);
			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			silhouetteAlpha = 0;

			secondsFont   = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 120);
			percentFont   = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 190);
			debugFontText = Font(loadFile(getAssetPath("fonts/MyriadPro-BlackSemiExt.ttf")), 26);

			Font percentTexureFont  = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 70);
			percentTexure = Utils::getTextField("%", &percentTexureFont,  Color::white());		

			setDetentionPercent(0);

			state = HIDING;
		}

		void init()
		{
			state = HIDING;
			timerVec              = Vec2f(1458.0f, 1080.0f);
			plashkaVec            = Vec2f(1920.0f, 0.0f);
			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			silhouetteAlpha = 0;
		}

		void draw()
		{
			gl::pushMatrices();
				/*#ifdef debug
					gl::translate( kinect().viewShiftX,  kinect().viewShiftY);
					gl::scale( kinect().headScale,  kinect().headScale);
					gl::color(ColorA(1.0f, 1.0f, 1.0f, silhouetteAlpha));
				#else
					gl::translate(Vec2f(0.0f, (1080.0f - 1440.0f) * 0.5f));
					gl::color(ColorA(1.0f, 1.0f, 1.0f, silhouetteAlpha));
				#endif	*/
				gl::translate(Vec2f(0.0f, (1080.0f - 1440.0f) * 0.5f));
				gl::color(ColorA(1.0f, 1.0f, 1.0f, silhouetteAlpha));
				if(currentPose)
					currentPose->draw();
			gl::popMatrices();

			gl::color(Color::white());
			/*gl::draw(sector, timerVec);
			
			gl::pushMatrices();
					gl::translate(timerVec);
					gl::translate(Vec2f(0.0f, 38.0f + 515.0f * 0.5f));
					gl::rotate(rotor);
					gl::translate(Vec2f(-515.0f * 0.5f, -515.0f * 0.5f));
					gl::draw(ciferblat);
			gl::popMatrices();*/

			/*gl::pushMatrices();
				gl::translate(timerVec);
				gl::translate(Vec2f(0.0f, 2.0f));
				gl::draw(arrow);
				gl::translate(Vec2f(75.0f, 40.0f));
				
				
				gl::color(Color::white());
			gl::popMatrices();*/
			gl::pushMatrices();
				gl::translate(timerVec);
				gl::draw(timer_bubble);
				string sec = to_string(showingSeconds);
				if (sec.size()<2)
					sec = "0" + sec;
				gl::Texture time = Utils::getTextField("00:" + sec, &secondsFont, Color::hex(0xc42f39));
				gl::translate(Vec2f(46.0f, 13.0f));				
				gl::draw(time);//, Vec2f(10.0f + 0.5f*(125 - time.getWidth()), 0.0f));

				gl::Texture percent = Utils::getTextField(to_string(percentMatching), &percentFont,  Color::white());				
				gl::color(Color::hex(0xc42f39));
				gl::translate(0.0, -200.0f );
				gl::draw(percent);
			gl::popMatrices();

			gl::color(Color::white());	
			gl::pushMatrices();
				gl::translate(plashkaVec);
				gl::draw(sidePlashka);
				gl::pushMatrices();
					gl::translate(5.0f,palkaPosition);				
					gl::draw(palka);
				gl::popMatrices();
				gl::translate(5.0f,1023.0f);
				gl::draw(popka);
			gl::popMatrices();
		/*
			float theta  = -sin(degree)/cos(degree);
			float angle  = 90 + 57.2957795f*atan(theta);
			if (theta > 0)	angle -=180;			
			
			gl::pushMatrices();				
				gl::translate(radius + shiftX, 1080.0f * 0.5f);
				gl::translate(-1756.0f, 0.0f);
				gl::translate(plashkaVec);
				gl::translate(radius*sin(degree), radius*cos(degree) );				
				gl::rotate(angle);		
				gl::translate(-palka.getWidth() * 0.5f, -palka.getHeight() * 0.5f);				
				gl::draw(palka);
			gl::popMatrices();		*/

			/*gl::pushMatrices();
				gl::translate(krugPercentAnimateVec);
				gl::translate(radius + shiftX - 170.0f, 1080.0f * 0.5f);
				gl::translate(radius*sin(degree1), radius * cos(degree1));				
				gl::color(Color::white());
				gl::translate(-krugPercent.getWidth() * 0.5f, -krugPercent.getHeight() * 0.5f );
				gl::draw(krugPercent);	

				gl::Texture percent = Utils::getTextField(to_string(percentMatching), &percentFont,  Color::white());
				float totalWidth = percent.getWidth() + percentTexure.getWidth();
				gl::color(Color::hex(0xc42f39));
				gl::translate(20.0f + (krugPercent.getWidth() - totalWidth) * 0.5f, 0.0f );
				gl::draw(percent);
				gl::translate(percent.getWidth() - 36.0f, 35.0f);
				gl::draw(percentTexure);
				gl::color(Color::white());		
			gl::popMatrices();*/

			if (matchingPopupAlpha > 0.0f)
			{
				gl::pushMatrices();
					gl::color(ColorA(1.0f, 1.0f, 1.0f, matchingPopupAlpha));
					gl::translate(Vec2f(1267.0f, 53.0f));
					gl::draw(matchingPopup);
					gl::translate(253.0f - 13.0f, 357.0f);
					int numActiveCircles = circlesNum*matchingProgress;
					for (int i = 0; i < circlesNum; i++)
					{
						gl::pushMatrices();							
						if (i+1<=numActiveCircles)
						{
							gl::translate(13.0f + 68.0f*i, 0.0f);
							gl::color(ColorA(196.0f/255.0f, 47.0f/255.0f, 57.0f/255.0f, matchingPopupAlpha));
							gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 24.0f, 20);
						}
						else
						{
							gl::translate(13.0f + 68.0f*i, 0.0f);
							gl::color(ColorA(223.0f/255.0f, 223.0f/255.0f, 223.0f/255.0f, matchingPopupAlpha));
							gl::drawSolidCircle(Vec2f(0.0f, 0.0f), 13.0f, 20);
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
			timeline().apply( &timerVec,Vec2f(1458.0f, 1080.0f), Vec2f(1458.0f, 868.0f), 1.2f, EaseOutCubic());
		}		

		void initStartAnimation()
		{
			rotor = 0;
			timeline().apply( &timerVec,Vec2f(1458.0f, 1080.0f), Vec2f(1458.0f, 868.0f), 1.2f, EaseOutCubic());
			timeline().apply( &plashkaVec, Vec2f(1920.0f, 0.0f), Vec2f(1831.0f , 0.0f), 1.2f, EaseOutCubic());
			timeline().apply( &krugPercentAnimateVec, Vec2f(300.0f, 0.0f), Vec2f(0.0f , 0.0f), 0.9f, EaseOutCubic());
		}

		void initStartAnimation1()
		{
			timeline().apply( &plashkaVec, Vec2f(1920.0f, 0.0f), Vec2f(1831.0f , 0.0f), 1.2f, EaseOutCubic() );
			timeline().apply( &krugPercentAnimateVec, Vec2f(300.0f, 0.0f), Vec2f(0.0f , 0.0f), 0.9f, EaseOutCubic());
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
			rotor += 0.8f;
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
			timeline().apply( &timerVec,    Vec2f(1458.0f, 1080.0f), 1.2f, EaseOutCubic());
			timeline().apply( &plashkaVec,  Vec2f(1920.0f, 0.0f), 1.2f, EaseOutCubic());
			timeline().apply( &krugPercentAnimateVec,  Vec2f(300.0f, 0.0f), 1.2f, EaseOutCubic());
			timeline().apply( &silhouetteAlpha, 1.0f, 0.0f, 0.4f, EaseInCubic());
			timeline().apply( &matchingPopupAlpha, 0.0f, 0.3f, EaseInCubic());
		}

		void setDetentionPercent(float percent)
		{
			if(percent > 1) percent = 1;
			if(percent < 0) percent = 0;

			percentMatching = (int)(percent*100);

			if(percentMatching>=100)
				percentMatching -= 1;

			palkaPosition = 884 + (8 - 884) * percent;
		}

		void showSilhouette()
		{
			#ifdef debug
				timeline().apply( &silhouetteAlpha, 0.0f, 0.6f, 0.4f, EaseInCubic() );
			#else
				timeline().apply( &silhouetteAlpha, 0.0f, 1.0f, 0.4f, EaseInCubic() );
			#endif
		}

		void setCurrentPose(Pose *pose)
		{
			currentPose = pose;
		}

		void showMatching(float  progressInt)
		{
			matchingPopupAlpha = (progressInt <= 0.01 ? 0.0f : 1.0f);
			matchingProgress = progressInt;
		}
};

inline GameControLScreen&	gameControls() { return GameControLScreen::getInstance(); };