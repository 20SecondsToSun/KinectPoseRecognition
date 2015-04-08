#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "CameraAdapter.h"
#include "AssetsManager.h"
#include "PlayerData.h"
#include "KinectAdapter.h"

class ComicsScreen
{
public:
	static ComicsScreen& getInstance() { static ComicsScreen game; return game; };

	void setup();	
	void createResultComicsFail(int index);
	void createResultComics(int index);
	void calculateFinalOffset();
	void drawAllGraphicsToFBO();
	void setPoseScale(float poseScale);
	void setPoseShift(const Vec2f& poseShiftVec);
	void setMiddlePoint(const Vec2f& poseShiftVec);
	void setCameraImage(const Texture& tex);
	void setComicsImage(const Texture& tex);
	void setGuess(bool guess);
	void draw();
	ci::gl::Texture getCurrentComics();	
	ci::gl::Texture getSuccessComics(int index);

private:
	bool guess;
	Fbo mainFbo;	
	int fboWidth, fboHeight;
	int currentSuccessIndex;
	float poseScale,  cameraImageScale;
	float scaleToFit;

	ci::Vec2f poseShiftVec, middleVec, finalShift;
	ci::gl::Texture cameraImage, comicsImage, failImage;
	ci::gl::Texture _successComics[2];	
};

inline ComicsScreen& comicsScreen() { return ComicsScreen::getInstance(); };