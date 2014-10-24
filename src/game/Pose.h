#pragma once

#include "Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace poseParams;

struct boxStruct 
{
	double x, y, z, w, h, d;
};

class Pose 
{
	public :
		void	setName(std::string _name)
		{ 
			name = _name;
		};

		void	setPoints(std::vector<ci::Vec3f> _rawPoints)
		{ 
			rawPoints = _rawPoints;
		};

		void	setPointColor(int i, Color color)
		{ 
			colors[i] = color;
		};

		void	initColors()
		{ 
			colors.clear();
			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				colors.push_back( Color(0, 0, 1));
			}			
		};		
		
		std::string	getName()  
		{
			return name;
		};

		std::vector<ci::Vec3f>	getPoints()
		{ 
			return rawPoints; 
		};

		std::vector<ci::Vec3f>	getNormalizePoints()
		{ 
			return normalizePoints; 
		};
		
		ci::Surface16u	getImage() 
		{ 
			return _image; 
		};

		ci::gl::Texture 	getImageTex() 
		{
			return _imageTex;
		};	

		void	setImage(ci::Surface16u val)  
		{ 
			_image = val; 
		};

		void	setImage(ci::gl::Texture val)  
		{ 
			_imageTex = val; 
		};

		void	draw()  
		{ 
			//gl::color(Color(1.0f, 0.0f, 0.0f));
			gl::draw(_imageTex);

			drawBox()	;

			drawAnchor();

		};

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
			for (size_t j = 0; j < rawPoints.size(); j++)
			{
				gl::pushMatrices();	
				gl::color(colors[j]);
				gl::drawSolidCircle( Vec2f(rawPoints[j].x, rawPoints[j].y), 3);	
				gl::popMatrices();
			}
		};

		void	drawBox()
		{
			//gl::color(Color(0.0f, 1.0f, 0.0f));			
			gl::drawStrokedRect(Rectf(boundingBox.x, boundingBox.y, boundingBox.x+boundingBox.w,  boundingBox.y+boundingBox.h));	
		}

		void	drawAnchor()
		{
			//gl::color(Color(0.0f, 0.0f, 1.0f));		
			gl::drawSolidCircle(anchorPoint, 3, 16);
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

		void	createNormalizePoints()
		{
			normalizePoints.clear();

			scaleTo();
			translateTo();
		}

		void scaleTo()
		{

			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				Vec3f v;				
				v.x = rawPoints[i].x * (BOX_SCALE/(/*rect.x -*/ boundingBox.w));
				v.y = rawPoints[i].y * (BOX_SCALE/(/*rect.y -*/ boundingBox.h));	
				//v.z = rawPoints[i].z * (BOX_SCALE/(/*rect.z -*/ boundingBox.d));
				normalizePoints.push_back(v);
			}
		}

		void translateTo()
		{
			Vec3f centroid = calculateCentroid();

			for (size_t i = 0; i < rawPoints.size(); i++)				
				normalizePoints[i]   +=  -centroid + Vec3f::one()*BOX_SCALE*0.5f;					
		}

		ci::Vec3f calculateCentroid()
		{			
			Vec3f xyz = Vec3f::zero();

			for (size_t i = 0; i < rawPoints.size(); i++)			
				xyz += rawPoints[i];				

			xyz/= rawPoints.size();		
	
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

	private:
		std::string				name;
		std::vector<ci::Vec3f>	rawPoints;
		std::vector<ci::Vec3f>	normalizePoints;
		std::vector<ci::Color>	colors;
		
	
		ci::Surface16u		_image;
		ci::gl::Texture     _imageTex;
		boxStruct			boundingBox;

		int32_t				tiltDegrees;
		ci::Vec2f			anchorPoint;

		ci::gl::Texture		comics;
};