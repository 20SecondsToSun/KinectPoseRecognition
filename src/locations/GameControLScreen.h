#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/app/AppNative.h"
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

		boost::signals2::signal<void(void )> timerOverEvent, quickAnimationFinishedEvent;	

		ci::Timer timer;
		int index;
		ci::Anim<float>	 alphaBubble;

		ci::gl::Texture sidePlashka, krugPercent, sector, ciferblat, arrow;

		ci::Anim<ci::Vec2f> timerVec, plashkaVec, krugPercentAnimateVec;

		ci::Font debugFont26, debugFontText, percentFont;
		int secondsToStop;

		float rotor;

		static const int SECONDS_FOR_START = 30;
		int showingSeconds, startingTime;

		ofxMSASpline2D spline2D;

		enum{
			RUNNING,
			FINISHING,
			HIDING,
			STATIC_SHOWING,
			QUICK_ANIMATION
		};

		int state;


		vector<Vec2f> animationPointsStates;
		Vec2f animationPosition;

		int percentMatching;


		gl::Texture percentTexure;


	


		float quickAnimPosePercent, qaPercent;
		int quickAnimTime;



		void setup()
		{
			sidePlashka   =  loadImage( loadAsset( "images/diz/sidePlashka.png" ));
			krugPercent   =  loadImage( loadAsset( "images/diz/krugPercent.png" ));


			sector   =  loadImage( loadAsset( "images/diz/sector.png" ));
			ciferblat   =  loadImage( loadAsset( "images/diz/ciferblat.png" ));
			arrow   =  loadImage( loadAsset( "images/diz/arrow.png" ));

			timerVec = Vec2f(-300.0f, 1076.0f);
			plashkaVec = Vec2f(2100.0f, 0.0f);
			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);

			debugFont26 = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 130);

			percentFont = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 150);

			debugFontText = Font(loadFile(getAssetPath("fonts/myriad/MyriadPro-BlackSemiExt.ttf")), 26);


			Font percentTexureFont  = Font(loadFile(getAssetPath("fonts/maestroc.ttf")), 70);
			percentTexure = Utils::getTextField("%", &percentTexureFont,  Color(1,1,1));

			secondsToStop = 10;

			spline2D.reserve(4);

			//spline2D.push_back(Vec2f(1838,12));
			//spline2D.push_back(Vec2f(1824,64));
			//spline2D.push_back(Vec2f(1803,102));
			spline2D.push_back(Vec2f(1794,148));

			//spline2D.push_back(Vec2f(1768,311));
			spline2D.push_back(Vec2f(1761,394));
			//spline2D.push_back(Vec2f(1761,475));
			//spline2D.push_back(Vec2f(1758,554));
			spline2D.push_back(Vec2f(1757,650));
			//spline2D.push_back(Vec2f(1765,728));
			//spline2D.push_back(Vec2f(1780,824));
			spline2D.push_back(Vec2f(1793,970));
			//spline2D.push_back(Vec2f(1853,1079));

			spline2D.setInterpolation(OFX_MSA_SPLINE_CUBIC);

			animationPointsStates = spline2D.getPoints(101);

			std::reverse(animationPointsStates.begin(),animationPointsStates.end());  

			setDetentionPercent(0);

			state = HIDING;
		}


		void draw()
		{
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

				gl::translate(Vec2f(4.0f, 148.0f));
				gl::color(Color::hex(0xff0000));
				//gl::drawSolidRect(Rectf(0,0,125,40));
				gl::color(Color::white());
				//gl::Texture txt;// = textFieldDraw(getCorrectSecondsText(showingSeconds), &debugFontText, Vec2f(0,0), Color(1,1,1));
				//if(state == QUICK_ANIMATION)
				//{
				//	 txt = textFieldDraw("яейсмд", &debugFontText, Color(1,1,1));
				//}
				//else
				//{
				//	txt = textFieldDraw(getCorrectSecondsText(showingSeconds), &debugFontText, Vec2f(0,0), Color(1,1,1));
				//}

				//gl::translate(0.5*(125 - txt.getWidth()), 0);
				//gl::draw(txt);

			gl::popMatrices();				

			
			gl::pushMatrices();
				gl::translate(plashkaVec);
				gl::draw(sidePlashka);
				//gl::translate(Vec2f(-100, 0));
				
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
				gl::translate(percent.getWidth()- 36, 28);
				gl::draw(percentTexure);
				gl::color(Color::white());


			
			gl::popMatrices();

			gl::pushMatrices();
				gl::translate(krugPercentAnimateVec);
				gl::translate(animationPosition);
				gl::translate(-krugPercent.getWidth()*0.5, -krugPercent.getHeight()*0.5 );
				
				Texture catTexture = Utils::getTextField("йнрнонгю", &debugFontText, Color(1,1,1));
				gl::color(Color::hex(0x9caab3));
				gl::translate((krugPercent.getWidth() - catTexture.getWidth())*0.5 - 2, 180 );
				gl::draw(catTexture);
				gl::color(Color::white());
			gl::popMatrices();
			
			//gl::color(Color::hex(0xff0000));		
			//spline2D.drawRaw();
			//spline2D.drawSmooth(101);

			gl::color(Color::white());

			console()<<" draw:::::::::::::"<<endl;
		}



		void init()
		{

		}

		void setTimeForAnimation(int seconds)
		{
			secondsToStop = seconds;
			showingSeconds = secondsToStop;
		}

		

		void show()
		{	
			console()<<" GAM ECONTROLS SHOW:::::::::::::"<< state<<endl;
			if (state == HIDING)
			{
				state  = STATIC_SHOWING;			
				initStartAnimation();	
			}
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
			//timer.start();
		}

		void hide()
		{
			timer.stop();
			state = HIDING;
			timeline().apply( &timerVec, Vec2f(-300.0f, 1076.0f), 0.9f, EaseOutCubic() );			
			timeline().apply( &plashkaVec,  Vec2f(2100.0f, 0.0f), 0.9f, EaseOutCubic() );
			timeline().apply( &krugPercentAnimateVec,  Vec2f(300.0f, 0.0f), 0.9f, EaseOutCubic() );
		}

		void initStartAnimation()
		{
			rotor = 0;
			timerVec = Vec2f(-300.0f, 1076.0f);
			timeline().apply( &timerVec, Vec2f(0.0f, 776.0f), 0.9f, EaseOutCubic() );

			plashkaVec = Vec2f(2100.0f, 0.0f);
			timeline().apply( &plashkaVec, Vec2f(1756.0f , 0.0f), 0.9f, EaseOutCubic() );//.finishFn( [ & ]( )



			krugPercentAnimateVec = Vec2f(300.0f, 0.0f);
			timeline().apply( &krugPercentAnimateVec, Vec2f(0.0f , 0.0f), 0.9f, EaseOutCubic() );
			//{
				
			//});	
		}
		
		

		void setDetentionPercent(float percent)
		{
			if(percent > 1) percent = 1;
			if(percent < 0) percent = 0;

			percentMatching = (int)(percent*100);
			animationPosition = animationPointsStates[percentMatching];			
		}


		void update()
		{
			switch (state)
			{
				case RUNNING:
					rotor+=0.8;
				break;

				default:
					break;
			}			
		}
		

		void animationFin()
		{
			
		}		

		void kill()
		{
		
		}

		std::string getCorrectSecondsText(int seconds)
		{
			string secText  = to_string(seconds);
			string memo = "яейсмд";
			if(secText == "1")
			{
				memo = "яейсмдю";
			}
			else if(secText == "2" || secText == "3"|| secText == "4")
			{
				memo = "яейсмдш";
			}
			else if(secText.size() == 2)
			{
				if(secText[0] == '1')
				{
					memo = "яейсмд";
				}
				else
				{
					if(secText[1] == '1')
					{
						memo = "яейсмдю";
					}
					else if(secText[1] == '0' || secText[1] >= '5')
					{
						memo = "яейсмд";
					}
					else
					{
						memo = "яейсмдш";
					}
				}				
			}	

			return memo;
		}


		
	

};

inline GameControLScreen&	gameControls() { return GameControLScreen::getInstance(); };