#include "KinectAdapter.h"
#include "Saver.h"

using namespace kinectDefaults;
KinectAdapter KinectAdapter::KinectAdapterState;

void KinectAdapter::setup()
{
	setActiveJoints();
	setDevice();
	calculateAspects();
	connect();

	poses = saver().loadPoseBase();

	reset();
}

void KinectAdapter::setActiveJoints()
{
	jointToRecord.push_back(NUI_SKELETON_POSITION_HEAD);
	jointToRecord.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	jointToRecord.push_back(NUI_SKELETON_POSITION_SHOULDER_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_SHOULDER_RIGHT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_ELBOW_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_ELBOW_RIGHT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_WRIST_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_WRIST_RIGHT);	
	jointToRecord.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	jointToRecord.push_back(NUI_SKELETON_POSITION_SPINE);

	/*jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_RIGHT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
	 
	jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_RIGHT);*/
}

void KinectAdapter::reset()
{
	currentPoseInGame  =  0;
	isTracking = true;	
	bufferDisconnect = 0;
}

void KinectAdapter::connect()
{	
	#ifdef kinectUsed
		kinectConnect();
	#endif	
}

void KinectAdapter::update() 
{
	_isConnected = (lastFrameId != frameID);
	lastFrameId = frameID;

	if (!_isConnected)
	{
		bufferDisconnect++;
	}
	else
		bufferDisconnect = 0;


	if (bufferDisconnect > 4)
	{
		_isConnected = false;

		if(!reconnectTimer.isStopped() && (getElapsedFrames() % 10 )== 0)
		{
			//console()<<"CONNECT!!!!!!!!!!!!!!"<<endl;
			connect();
		}
		else if (reconnectTimer.isStopped())
		{
			reconnectTimer.start();
		}
	}
	else
	{
		_isConnected = true;
		if (!reconnectTimer.isStopped())		
			reconnectTimer.stop();		
	}
}

void KinectAdapter::updateGame() 
{	
	if (!_isConnected) return;

	if (isTracking)
		updateSkeletonData();

	#ifndef recording
		if (isGameRunning)	
		{
			matchTemplate();

			if (mathPercent > Params::percentForMatching )		
			{
				levelCompletion += 4;
			}
			else
			{
				if(levelCompletion - 6 >=0)
					levelCompletion -= 6;
				else
					levelCompletion = 0;
			}
		}
	#endif
}

void KinectAdapter::poseComplete() 
{
	isPoseDetecting = true;
	levelCompletion = 0;
}

float KinectAdapter::getPoseProgress() 
{
	return floor(levelCompletion);
}

void KinectAdapter::updateSkeletonData()
{		
	currentSkelet.clear();
	//int k = 0;
	
	for ( const auto& skeleton : mFrame.getSkeletons() ) 
	{	
		if (skeleton.size() == 0 ) continue;
		
		for (size_t i = 0, len = jointToRecord.size(); i < len; i++)
		{
			if(skeleton.find(jointToRecord[i]) != skeleton.end())
			{
				const MsKinect::Bone& bone = skeleton.find(jointToRecord[i])->second;
				
				Vec2f v0				= mDevice->mapSkeletonCoordToColor( bone.getPosition() );
				auto  z					= skeleton.at( bone.getStartJoint()).getPosition().z;			
				currentSkelet.push_back(Vec3f(v0.x, v0.y, z));	
			}
		}

		if (currentSkelet.size()!= 0 ) break;
	}
	
	if(getSurface8u() && getDepthChannel16u() && currentSkelet.size()!=0 && mDevice->isCapturing())	
		savePoseDepth = MsKinect::greenScreenUsers(getDepthChannel16u(), getSurface8u(), COLOR_RESOLUTION, DEPTH_RESOLUTION);	
}

int KinectAdapter::getSkeletsInFrame()
{
	if(getDepthChannel16u())
		return MsKinect::calcNumUsersFromDepth(getDepthChannel16u());

	return 0;
}

void KinectAdapter::draw()
{
	gl::color(Color::white());	

	#ifdef recording	
		//gl::color(ColorA(1,1,1,0.5));
		//drawKinectCameraColorSurface();
		//gl::color(ColorA(1,1,1, 1));
		
		drawUserMask();
		drawSkeletJoints();	
	#else 	
		//drawKinectCameraColorSurface();
		drawLoadedPoses();
		drawSkeletJoints();
	#endif
}

void KinectAdapter::drawLoadedPoses()
{
	gl::enableAlphaBlending();
	gl::pushMatrices();
		gl::translate(viewShiftX, viewShiftY);
		gl::scale(headScale, headScale);		
		poses[currentPoseInGame]->draw();
	gl::popMatrices();
}

void KinectAdapter::drawSkeletJoints()
{
	gl::pushMatrices();
		gl::translate(viewShiftX, viewShiftY);
		gl::scale(headScale, headScale);
		currentPose.drawPoints();
		currentPose.drawBox();
		currentPose.drawAnchor();
	gl::popMatrices();
}

void KinectAdapter::drawUserMask()
{
	if(savePoseDepth)
	{
		gl::color(Color::white());
		gl::enableAlphaBlending();
		
		gl::pushMatrices();
			gl::translate(viewShiftX, viewShiftY);
			gl::scale(headScale, headScale);			
			gl::draw(Texture(savePoseDepth));
		gl::popMatrices();
	}
}

void KinectAdapter::drawPoseComics() 
{

}

void KinectAdapter::startPoseGame()
{
	level = 0;
	currentPoseInGame = generatePoseNum();
}

int KinectAdapter::nextPose()
{	
	level++;
	currentPoseInGame = generatePoseNum();	

	if (level >= POSE_IN_GAME_TOTAL)
	{
		level = 0;
		currentPoseInGame = generatePoseNum();
		gameOverEvent();
	}

	return currentPoseInGame;
}

int KinectAdapter::getPoseCode()
{
	return currentPoseInGame;
}

int KinectAdapter::generatePoseNum()
{
	return level;
}

ci::gl::Texture KinectAdapter::getPoseImage()
{
	return poses[currentPoseInGame]->getComicsImage();
}

ci::gl::Texture KinectAdapter::getPoseImageById(int id)
{
	return poses[id]->getComicsImage();
}

string KinectAdapter::getPoseIndex()
{
	return to_string(poses.size() + 1);
}

void KinectAdapter::saveAsTemplate(string name)
{
	if (currentSkelet.size() && savePoseDepth) 
	{
		Pose* pose = new Pose();
		pose->setName(name);
		pose->setPoints(currentSkelet);
		pose->setKinectTilt(getTilt());
		pose->setImage(savePoseDepth);
		saver().savePoseIntoBase(pose);		
	}
}

void KinectAdapter::matchTemplate()
{
	foundPose				 = NULL;

	currentPose.setPoints(currentSkelet);
	currentPose.createBoundingBox();
	currentPose.createNormalizePoints();
	currentPose.initColors();

	if(!isPointsSizeEqual(*poses[currentPoseInGame], currentPose))
	{
		mathPercent = 0;
		return;
	}	

	if (!isAnchorPointsMatch(*poses[currentPoseInGame], currentPose))
	{
		mathPercent = 0;
		return;
	}	

	//computeMistakeWay1();
	computeMistakeWay2();
}

void KinectAdapter::computeMistakeWay1()
{
	double min_dist			 = FLT_MAX;	
	double maxErrorToDiscard = 1000;
	double sum_mistake		 = 0;

	for(size_t j = 0, len = currentPose.getNormalizePoints().size(); j < len ; ++j) 
	{
		double mistake = calculateDistanceBetweenPoints(poses[currentPoseInGame]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
	
		sum_mistake += mistake;

		if (mistake > Params::maxErrorBetweenJoints) 
		{
			currentPose.setPointColor(j, Color(1,1,1));
			//mathPercent = 0;
			//return;
			//break;
		}
	}

	double dist = sum_mistake/currentPose.getNormalizePoints().size();

	if(dist >= 0 && dist < min_dist) 
	{
		min_dist = dist;
		foundPose = poses[currentPoseInGame];
	}

	double kv = BOX_SCALE*BOX_SCALE;
	double half_diagonal = 0.5 * sqrt(kv + kv); 
	mathPercent = 1.0 - (min_dist/half_diagonal);	

	mathPercent < 0 ? mathPercent = 0 : mathPercent = mathPercent;
}

void KinectAdapter::computeMistakeWay2()
{
	mathPercent = 0;
	size_t len = currentPose.getNormalizePoints().size();
	double onePart = 100.0 / len;

	for(size_t j = 0; j < len ; ++j) 
	{
		double mistake = calculateDistanceBetweenPoints(poses[currentPoseInGame]->getNormalizePoints()[j], currentPose.getNormalizePoints()[j]);
		double onePartPercent = 0;

		if (mistake >= Params::maxErrorBetweenJoints) 
		{
			onePartPercent = 0;
		}
		else
		{			
			onePartPercent = onePart * (1-mistake/Params::maxErrorBetweenJoints);
		}
		mathPercent += onePartPercent;
	}
	mathPercent*=0.01;
}

double KinectAdapter::getMatchPercent()
{
	return mathPercent*100;
}

bool KinectAdapter::isPointsSizeEqual(Pose pose1, Pose pose2)
{
	return pose1.getNormalizePoints().size() == pose2.getNormalizePoints().size();
}

bool KinectAdapter::isAnchorPointsMatch(Pose pose1, Pose pose2)
{
	Vec2f anchor1 = pose1.getAnchor();
	Vec2f anchor2 = pose2.getAnchor();

	return (abs(anchor1.x - anchor2.x) < Params::boxMaxErrorX && abs(anchor1.y -anchor2.y) < Params::boxMaxErrorY  );
}

double KinectAdapter::calculateDistanceBetweenPoints(Vec3f vec1, Vec3f vec2)
{
	return (Vec2f(vec1.x, vec1.y) - Vec2f(vec2.x, vec2.y) ).length();
}