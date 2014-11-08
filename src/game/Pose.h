#pragma once
#pragma warning(push)
#pragma warning(disable: 4244)

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
		boxStruct boundingBox;

		void	setName(std::string _name)
		{ 
			name = _name;
		};

		void	setPoints(std::vector<ci::Vec3f> _rawPoints)
		{ 
			rawPoints = _rawPoints;
			drawingPoints = _rawPoints;
			//initColors();
		};

		void	setRawPoints(std::vector<ci::Vec3f> _rawPoints)
		{ 
			rawPoints = _rawPoints;
			//initColors();
		};


		void	setDrawPoints(std::vector<ci::Vec3f> _drawPoints)
		{ 
			drawingPoints = _drawPoints;
			//initColors();
		};



		void createSkeletBoundingBox(double _width, double _height)
		{
			boundingBox.w = _width;
			boundingBox.h = _height;
			boundingBox.x = 0;
			boundingBox.y = 0;
		}

		void createSelfSkeletBoundingBox()
		{
			double skeletFullHeight = 0;
			double skeletFullWidth = 0;
			Vec3f sholderVec, hipCenterVec, elbowLeftVec, sholderLeftVec;

			for (size_t i = 0, len = rawPoints.size(); i < len; i++)
			{			
				Vec3f jnt =   Vec3f(rawPoints[i].x, rawPoints[i].y, rawPoints[i].z);
				
				if (i == 0)
					hipCenterVec  = jnt;
				else if (i == 2)
				{
					sholderVec  = jnt;
					//skeletFullHeight += hipCenterVec.distance(jnt);
				}
				else if (i == 3)
				{
					skeletFullHeight += sholderVec.distance(jnt);
				}
				else if (i == 4)
				{
					skeletFullWidth += sholderVec.distance(jnt);
					sholderLeftVec = jnt;
				}
				else if (i == 5)
				{
					skeletFullWidth += sholderLeftVec.distance(jnt);
					elbowLeftVec = jnt;
				}
				else if (i == 6)
				{
					skeletFullWidth += elbowLeftVec.distance(jnt);
				}
			}
			skeletFullWidth *= 2;


			console()<<"init width ::::::::::::: "<<skeletFullWidth<<endl;
			boundingBox.w = skeletFullWidth;
			boundingBox.h = skeletFullHeight;
			boundingBox.x = 0;
			boundingBox.y = 0;
		}

		void	setPointColor(int i, ColorA color)
		{ 
			colors[i] = color;
		};

		void	setRedColors()
		{ 
			colors.clear();
			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				colors.push_back( ColorA(1.0f, 0.0f, 0.0f, 1.0f));
			}			
		};		

		void	initColors()
		{ 
			colors.clear();
			for (size_t i = 0; i < rawPoints.size(); i++)
			{
				colors.push_back( ColorA(0.0f, 0.0f, 1.0f, 1.0f));
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
		};

		void	draw()  
		{ 
			//gl::color(Color(1.0f, 0.0f, 0.0f));
			gl::draw(_imageTex);
		//	drawPoints();
			//drawBox()	;
			//drawAnchor();

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

			for (size_t j = 0; j < normalizePoints.size(); j++)
			{
				gl::pushMatrices();	
				gl::color(colors[j]);
				Vec2f vec =  Vec2f(normalizePoints[j].x, normalizePoints[j].y);
				gl::drawSolidCircle(vec, 7.0f);
				gl::popMatrices();
				gl::color(Color::white());
			}
		};

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
		int _timeForPose, _matchPercent;
		int time;
};
#pragma warning(pop)