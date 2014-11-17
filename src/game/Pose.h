#pragma once
#pragma warning(push)
#pragma warning(disable: 4244)

#include "Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace poseParams;

class Pose 
{
	public :
		
		struct boxStruct 
		{
			double x, y, z, w, h, d;
		}  boundingBox;

		Vec2f leftFootVec, rightFootVec;
		float middleX, middleY;

		void	setName(std::string _name)
		{ 
			name = _name;
		};

		void	setPoints(std::vector<ci::Vec3f> _rawPoints)
		{ 
			rawPoints = _rawPoints;
			drawingPoints = _rawPoints;	
		}

		void	setRawPoints(std::vector<ci::Vec3f> _rawPoints)
		{ 
			rawPoints = _rawPoints;		
		}

		void setDrawPoints(std::vector<ci::Vec3f> _drawPoints)
		{ 
			drawingPoints = _drawPoints;
			
		}

		void	scalePoints(float _scale)
		{
			for (int i = 0; i < rawPoints.size(); i++)
			{
				normalizePoints[i].x = rawPoints[i].x *_scale;
				normalizePoints[i].y = rawPoints[i].y *_scale;

				normalizePoints[i].x += middleX;//(640 - 640*_scale)*0.5;
				normalizePoints[i].y += middleY;//480 - 480*_scale;
			}
		}

		
		Vec2f getPoseShift()
		{
			return Vec2f(middleX, middleY);
		}
		
		void calculateShifts(float _scale)
		{
			int leftFootIndex = normalizePoints.size() - 1;
			float leftFootShiftX = normalizePoints[leftFootIndex].x - normalizePoints[leftFootIndex].x *_scale;
			float leftFootShiftY = normalizePoints[leftFootIndex].y - normalizePoints[leftFootIndex].y *_scale;

			int rightFootIndex = normalizePoints.size() - 3;
			float rightFootShiftX = normalizePoints[rightFootIndex].x - normalizePoints[rightFootIndex].x *_scale;
			float rightFootShiftY = normalizePoints[rightFootIndex].y - normalizePoints[rightFootIndex].y *_scale;

			leftFootVec = Vec2f(leftFootShiftX, leftFootShiftY);
			rightFootVec = Vec2f(rightFootShiftX, rightFootShiftY);

			middleX = (leftFootVec.x + rightFootVec.x ) * 0.5;
			middleY = ( leftFootVec.y > rightFootVec.y ) ? leftFootVec.y  : rightFootVec.y ;
			console()<<" middleX::  "<< middleX<<"  middleY  "<<middleY<< endl;
		}
		
		void	drawRawPoints()
		{
			for (size_t j = 0; j < rawPoints.size(); j++)
			{
				gl::pushMatrices();				
				Vec2f vec =  Vec2f(rawPoints[j].x, rawPoints[j].y);

				if (j == rawPoints.size() - 3)
					gl::color(Color::hex(0x5820e3));

				if (j == rawPoints.size() - 1)
					gl::color(Color::hex(0xe32065));

				gl::drawSolidCircle(vec, 7.0f);
				gl::popMatrices();
			}
			gl::color(Color::white());
		}

		void createSkeletBoundingBox(double _width, double _height)
		{
			boundingBox.w = _width;
			boundingBox.h = _height;
			boundingBox.x = 0;
			boundingBox.y = 0;
		}

		void	setPointColor(int i, ColorA color)
		{ 
			colors[i] = color;
		}

		void	setRedColors()
		{ 
			colors.clear();
			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				colors.push_back( ColorA(1.0f, 0.0f, 0.0f, 1.0f));
			}			
		}	

		void	initColors()
		{ 
			colors.clear();
			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				colors.push_back( ColorA(0.0f, 0.0f, 1.0f, 1.0f));
			}			
		}	
		
		std::string	getName()  
		{
			return name;
		}

		std::vector<ci::Vec3f>	getPoints()
		{ 
			return rawPoints; 
		}

		std::vector<ci::Vec3f>	getNormalizePoints()
		{ 
			return normalizePoints; 
		}
		
		ci::Surface16u	getImage() 
		{ 
			return _image; 
		}

		ci::gl::Texture 	getImageTex() 
		{
			return _imageTex;
		}

		void	setImage(ci::Surface16u val)  
		{ 
			_image = val; 
		}

		void setTime(int val)  
		{
			_timeForPose = val;
		}

		int getTime()  
		{
			return _timeForPose;
		}

		void setPercent(int val)  
		{
			_matchPercent = val;
		}

		int getPercent()  
		{
			return _matchPercent;
		}		

		void setComicsName(std::string val)  
		{
			_comicsName = val;
		}			

		std::string getComicsName()  
		{
			return _comicsName;
		}	

		void	setImage(ci::gl::Texture val)  
		{ 
			_imageTex = val; 
		}

		void	draw()  
		{ 			
			gl::draw(_imageTex);
		}

		void	setComicsImage(ci::gl::Texture _comics)  
		{
			comics = _comics;
		}

		ci::gl::Texture		getComicsImage()  
		{
			return comics;
		}		

		void	drawPoints()
		{
			for (size_t j = 0; j < normalizePoints.size(); j++)
			{
				gl::pushMatrices();	
				gl::color(colors[j]);
				Vec2f vec =  Vec2f(normalizePoints[j].x, normalizePoints[j].y);
				gl::drawSolidCircle(vec, 4.0f);
				gl::popMatrices();
				gl::color(Color::white());
			}
		}		

		void	drawBox()
		{
			//gl::color(Color(0.0f, 1.0f, 0.0f));	
			gl::pushMatrices();	
			//gl::scale(200, 200);
			gl::drawStrokedRect(Rectf((float)boundingBox.x, (float)boundingBox.y, (float)(boundingBox.x+boundingBox.w),  (float)(boundingBox.y+boundingBox.h)));	
			gl::popMatrices();
		}

		void	drawAnchor()
		{
			//gl::color(Color(0.0f, 0.0f, 1.0f));		
			gl::drawSolidCircle(anchorPoint, 3.0f, 16);
		}

		void	drawLabel() {};		

		ci::Vec2f	getAnchor()
		{
			return anchorPoint;
		}

		void	createBoundingBox()
		{
			double min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN, min_z = FLT_MAX, max_z = FLT_MIN;
	
			for (size_t i = 0; i < rawPoints.size(); i++)
			{

				Vec3f v = rawPoints[i];
				if(v.x < min_x) min_x = v.x;
				if(v.x > max_x) max_x = v.x;
				if(v.y < min_y) min_y = v.y;
				if(v.y > max_y) max_y = v.y;	
				if(v.z < min_z) min_z = v.z;
				if(v.z > max_z) max_z = v.z;
			}
			
			boundingBox.x = min_x;
			boundingBox.y = min_y;
			boundingBox.z = min_z;
	
			boundingBox.w = (max_x - min_x);
			boundingBox.h = (max_y - min_y);
			boundingBox.d = (max_z - min_z);

			anchorPoint = Vec2f(boundingBox.x, boundingBox.y+boundingBox.h);
		}

	/*	float calculateUserHeight()
		{
			if( rawPoints.size()<12) return 0.0f;
			int leftFootIndex = normalizePoints.size() - 1;
			float leftFootY = normalizePoints[leftFootIndex].y;

			int rightFootIndex = normalizePoints.size() - 3;
			float rightFootY = normalizePoints[rightFootIndex].y;

			int headIndex = 1;
			float headY = normalizePoints[headIndex].y;

			return abs(headY - leftFootY);
		}*/

		void setBoundingBox(boxStruct _boundingBox)
		{
			boundingBox = _boundingBox;
		}

		void createNormalizePoints0()
		{
			normalizePoints  = rawPoints;
		}

		void createNormalizePoints()
		{
			normalizePoints  = rawPoints;
		
			//normalizePoints.clear();
			scaleTo();
			translateTo();
		}

		void scaleTo()
		{
			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				Vec3f v;
				v.x = rawPoints[i].x * (BOX_SCALE / boundingBox.w);
				v.y = rawPoints[i].y * (BOX_SCALE / boundingBox.h);
				normalizePoints.push_back(v);
			}
		}

		void translateTo()
		{
			Vec3f centroid = calculateCentroid();

			for (size_t i = 0; i < rawPoints.size(); i++)
				normalizePoints[i] += -centroid + Vec3f::one() * BOX_SCALE * 0.5f;
		}

		ci::Vec3f calculateCentroid()
		{			
			Vec3f xyz = Vec3f::zero();

			for (size_t i = 0; i < rawPoints.size(); i++)			
				xyz += rawPoints[i];				

			xyz /= rawPoints.size();		
	
			return xyz;
		}

		void setKinectTilt(int32_t _tiltDegrees)
		{
			tiltDegrees = _tiltDegrees;
		}

		int32_t getKinectTilt()
		{
			return tiltDegrees;
		}

		void setPoseTime(int32_t _time)
		{
			_timeForPose = _time;
		}

		int32_t getPoseTime()
		{
			return _timeForPose;
		}

	private:
		std::string				name;
		std::vector<ci::Vec3f>	rawPoints;
		std::vector<ci::Vec3f>	normalizePoints;
		std::vector<ci::Vec3f>	drawingPoints;
		std::vector<ci::ColorA>	colors;		
	
		ci::Surface16u _image;
		ci::gl::Texture _imageTex;		

		int32_t tiltDegrees;
		ci::Vec2f anchorPoint;

		ci::gl::Texture comics;

		std::string _comicsName;
		int _timeForPose, _matchPercent, time;
};
#pragma warning(pop)