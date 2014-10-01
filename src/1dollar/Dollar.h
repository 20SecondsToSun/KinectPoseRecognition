#pragma once;
#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "cinder/CinderMath.h"
#include "Utils.h"

#define golden_ratio  0.5 * (-1.0 + sqrt(5.0))
#define PI 3.1415

class Gesture;

namespace dollarDefaults
{
	static const std::string  DATA_BASE_NAME =  "gesture.json";
	static const float  BOX_SCALE			 =	250.0;

}

class Dollar 
{
	public :
		Dollar();

		void loadGestureBase(std::string& filePath);
		void loadGestureBase();

		void addGesture(Gesture* gesture);

		void saveGestureToBase();

		void findInGestures(std::vector<ci::Vec3f> rawPoints);

		void startRecord();
		void stopRecord();
		void recording(ci::Vec3f point);		

		Gesture* matchGesture(Gesture* gesture, double *pScore);

		std::vector<ci::Vec3f> rawData;

		void draw();
		void drawDataBaseGestures();
		void drawStatusBar();

		bool isDrawDataBaseGestures;

		void execute(char symbol);

	private :
		std::vector<Gesture*> gestures;
		

		bool isRecording;

		int num_samples;
		float square_size;
		float angle_precision;
		double	half_diagonal;

		std::map <std::string, Gesture*> uniqueGesturesInBase;

		ci::Font hintFont;

		std::string currentState;

};

class Gesture 
{
	public :
		void	setName(std::string _name){ name = _name;};
		void	setPoints(std::vector<ci::Vec3f> _rawPoints){ rawPoints = _rawPoints; resampled_points = _rawPoints;};
		void	setAnglePrecision(float _angle_precision){ angle_precision = _angle_precision;};	

		std::string	getName()  { return name; };
		std::vector<ci::Vec3f>	getPoints(){ return resampled_points; };

		void	normalize(int num_samples);// private


		std::vector<ci::Vec3f> resampled_points;// private
		std::vector<ci::Vec3f> rotated_points;// private		

		float	angle_precision;

		ci::Vec3f centroid;// private

		double distanceAtBestAngle(Gesture* pGesture) ;

		void drawGestures();

	private:
		double length();

		void resample(int nNumSamples);
		void rotateToZero();
		void scaleTo(double nSize = dollarDefaults::BOX_SCALE);
		void translate();

		double indicativeAngle();
		std::vector<ci::Vec3f> Gesture::rotateBy(std::vector<ci::Vec3f> oPoints, double angle);

		void	calculateCentroid();

		ci::Rectf boundingBox() ;

	

		std::string name;
		std::vector<ci::Vec3f> rawPoints;


		
		double distanceAtAngle(double nAngle, Gesture* pGesture);
		double pathDistance(std::vector<ci::Vec3f> oPoints, Gesture* pGesture);
		
		
};