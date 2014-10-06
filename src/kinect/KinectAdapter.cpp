#include "KinectAdapter.h"
#include "Saver.h"

using namespace kinectDefaults;

KinectAdapter KinectAdapter::KinectAdapterState;

void KinectAdapter::Setup()
{	
	currentPoseInGame  =  0;
	isTracking = true;

	setActiveJoints();

	setDevice();

	calculateAspects();

	kinectConnect();

	poses = saver().loadPoseBase();
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

void KinectAdapter::update() 
{
	if (isTracking)
		updateSkeletonData();

	#ifndef recording
		if (isGameRunning)	
		{
			matchTemplate();

			if (mathPercent > Params::percentForMatching )		
			{
				levelCompletion +=2;
			}
			else
			{
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

	for ( const auto& skeleton : mFrame.getSkeletons() ) 
	{		
		for (size_t i = 0; i < jointToRecord.size(); i++)
		{
			if(skeleton.find(jointToRecord[i]) != skeleton.end())
				{
				const MsKinect::Bone& bone = skeleton.find(jointToRecord[i])->second;
				Vec2f v0				   = mDevice->mapSkeletonCoordToDepth( bone.getPosition() );
				auto  z					   = skeleton.at( bone.getStartJoint()).getPosition().z;			
				currentSkelet.push_back(Vec3f(v0.x, v0.y, z));				
			}
		}		
	}

	if(mDepthChannel16u)
		savePoseDepth = MsKinect::greenScreenUsers(mDepthChannel16u, surface8u, COLOR_RESOLUTION, DEPTH_RESOLUTION);		
}

int KinectAdapter::getSkeletsInFrame()
{
	if(mDepthChannel16u)
		return MsKinect::calcNumUsersFromDepth(mDepthChannel16u);

	return 0;
}

void KinectAdapter::draw()
{
	gl::color(Color::white());
	drawKinectCameraColorSurface();

	#ifdef recording		
		drawUserMask();
		drawSkeletJoints();	
	#else 	
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
		
		for (size_t i = 0; i < currentSkelet.size(); i++)		
			gl::drawSolidCircle( Vec2f(currentSkelet[i].x, currentSkelet[i].y), 8.0f/headScale, 16 );		
			
	gl::popMatrices();
}

void KinectAdapter::drawUserMask()
{
	if(mDepthChannel16u)
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
	currentPoseInGame = 0;
}

void KinectAdapter::nextPose()
{	
	currentPoseInGame++;

	if (currentPoseInGame >= poses.size())
	{
		currentPoseInGame = 0;
		gameOverEvent();
	}
}

string KinectAdapter::getPoseIndex()
{
	return to_string(poses.size() + 1);
}

void KinectAdapter::saveAsTemplate(string name)
{
	if (currentSkelet.size()) 
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

	double min_dist			 = FLT_MAX;	
	double maxErrorToDiscard = 1000;
	double sum_mistake		 = 0;	

	Pose currentPose;
	currentPose.setPoints(currentSkelet);
	currentPose.createBoundingBox();
	currentPose.createNormalizePoints();		



	if(poses[currentPoseInGame]->getPoints().size() != currentSkelet.size())
	{
		mathPercent = 0;
		return;
	}	

	if (isAnchorPointsMath(*poses[currentPoseInGame], currentPose))
	{
		mathPercent = 0;
		return;
	}		
	
	for(size_t j = 0; j < currentSkelet.size(); ++j) 
	{
		double mistake = calculateDistanceBetweenPoints(poses[currentPoseInGame]->getPoints()[j], currentPose.getPoints()[j]);
	
		sum_mistake += mistake;

		if (mistake > Params::maxErrorBetweenJoints) 
		{
			mathPercent = 0;
			return;
		}
	}

	double dist = sum_mistake/currentSkelet.size();

	if(dist > 0 && dist < min_dist) 
	{
		min_dist = dist;
		foundPose = poses[currentPoseInGame];
	}

	double half_diagonal = 0.5 * sqrt((BOX_SCALE*BOX_SCALE) + (BOX_SCALE*BOX_SCALE));//+ (BOX_SCALE*BOX_SCALE));	 
	mathPercent = 1.0 - (min_dist/half_diagonal);	
}

double KinectAdapter::getMatchPercent()
{
	return mathPercent*100;
}

bool KinectAdapter::isAnchorPointsMath(Pose pose1, Pose pose2)
{
	Vec2f anchor1 = pose1.getAnchor();
	Vec2f anchor2 = pose2.getAnchor();

	return (abs(anchor1.x - anchor2.x) > Params::boxMaxErrorX || abs(anchor1.y -anchor2.y) > Params::boxMaxErrorY  );
}

double KinectAdapter::calculateDistanceBetweenPoints(Vec3f vec1, Vec3f vec2)
{
	return (Vec2f(vec1.x, vec1.y) - Vec2f(vec2.x, vec2.y) ).length();
}