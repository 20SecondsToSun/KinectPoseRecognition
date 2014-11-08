#include "KinectAdapter.h"
#include "Saver.h"

using namespace kinectDefaults;
KinectAdapter KinectAdapter::KinectAdapterState;

void KinectAdapter::setup()
{
	bufferDisconnect = 0;
	setActiveJoints();
	setDevice();
	calculateAspects();
	kinectConnect();
}

void KinectAdapter::setActiveJoints()
{	
	//jointToRecord.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	jointToRecord.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);// 0.1
	jointToRecord.push_back(NUI_SKELETON_POSITION_HEAD);	//.1

	jointToRecord.push_back(NUI_SKELETON_POSITION_SHOULDER_LEFT);//0.05
	jointToRecord.push_back(NUI_SKELETON_POSITION_ELBOW_LEFT);//0.05

	jointToRecord.push_back(NUI_SKELETON_POSITION_WRIST_LEFT);//0.3

	jointToRecord.push_back(NUI_SKELETON_POSITION_SHOULDER_RIGHT);//0.05	
	jointToRecord.push_back(NUI_SKELETON_POSITION_ELBOW_RIGHT);	//0.05

	jointToRecord.push_back(NUI_SKELETON_POSITION_WRIST_RIGHT);//	0.3

	if (Params::isFullSkelet)
	{
		jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
		jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
		jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_RIGHT);	
		jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_RIGHT);
	}
}

void KinectAdapter::update() 
{
	bool prevConnectedValue = _isConnected;
	_isConnected = mDevice->isDeviceRunning();

	if (prevConnectedValue == false && _isConnected == true)
		kinectConnectedEvent();

	if (!sleepTimer.isStopped() && sleepTimer.getSeconds() > sleepSeconds)
		sleepTimer.stop();
}

void KinectAdapter::updateSkeletonData()
{	
	if (!isTracking || !_isConnected) return;

	currentSkelet.clear();

	for ( const auto& skeleton : mFrame.getSkeletons() ) 
	{
		if (skeleton.size() == 0 ) continue;

		skeletFullHeight = 0;
		skeletFullWidth = 0;
		Vec3f sholderVec, hipCenterVec, elbowLeftVec, sholderLeftVec;	

		for (size_t i = 0, len = jointToRecord.size(); i < len; i++)
		{
			auto joint = skeleton.find(jointToRecord[i]);
			if ( joint != skeleton.end())
			{
				const MsKinect::Bone& bone = joint->second;

				Vec2f v0 = mDevice->mapSkeletonCoordToColor( bone.getPosition());
				auto  z = skeleton.at( bone.getStartJoint()).getPosition().z;
				currentSkelet.push_back( Vec3f(v0.x, v0.y, z));
			}
		}

		skeletFullWidth *= 2;

		break;
	}

	if(getSurface8u() && getDepthChannel16u() && currentSkelet.size()!=0 && mDevice->isCapturing())	
		savePoseDepth = MsKinect::greenScreenUsers(getDepthChannel16u(), getSurface8u(), COLOR_RESOLUTION, DEPTH_RESOLUTION);	
}

ci::Surface16u KinectAdapter::getSilhouette()
{
	return savePoseDepth;
}

int KinectAdapter::getSkeletsInFrame()
{
	int peopleInFrame = 0 ;

	if (!sleepTimer.isStopped())
	{
		if (sleepTimer.getSeconds() > sleepSeconds)
			sleepTimer.stop();

		peopleInFrame =  0;
	}
	else if(getDepthChannel16u())
	{
		peopleInFrame = MsKinect::calcNumUsersFromDepth(getDepthChannel16u());
	}

	/*#ifdef debug
	peopleInFrame = 1; 
	#endif*/

	return peopleInFrame;
}

float KinectAdapter::distanceToSkelet()
{
	float distance;

	if (currentSkelet.size() == 0) 
	{
		distance = 0;
	}
	else
	{
		distance = currentSkelet[1].z;
	}

	return distance;
}

void KinectAdapter::draw()
{
	gl::color(Color::white());	

#ifdef recording	
	//gl::color(ColorA(1,1,1,0.5));

	// drawKinectCameraColorSurface();
	//gl::color(ColorA(1,1,1, 1));

	drawUserMask();
	drawSkeletJoints();	
#endif

#ifdef debug	
	//drawKinectCameraColorSurface();
#endif
}


void KinectAdapter::drawSkeletJoints()
{
	gl::pushMatrices();
	gl::translate(viewShiftX, viewShiftY);
	gl::scale(headScale, headScale);
	//currentPose.drawPoints();
	//currentPose.drawBox();
	//currentPose.drawAnchor();
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
		//gl::draw(Texture(getSurface8u()));
		gl::draw(Texture(savePoseDepth));
		gl::popMatrices();
	}
}

void KinectAdapter::sleep(int seconds)
{
	sleepSeconds = seconds;
	sleepTimer.start();
}

void KinectAdapter::sleepKill( )
{
	sleepTimer.stop();
}
//jnt =   Vec3f(v0.x, v0.y, z);
//
//if (jointToRecord[i] == NUI_SKELETON_POSITION_HIP_CENTER)
//	hipCenterVec  = jnt;
//else if (jointToRecord[i] == NUI_SKELETON_POSITION_SHOULDER_CENTER)
//{
//	sholderVec  = jnt;
//	//skeletFullHeight += hipCenterVec.distance(jnt);
//}
//else if (jointToRecord[i] == NUI_SKELETON_POSITION_HEAD)
//{
//	skeletFullHeight += sholderVec.distance(jnt);
//}
//else if (jointToRecord[i] == NUI_SKELETON_POSITION_SHOULDER_LEFT)
//{
//	skeletFullWidth += sholderVec.distance(jnt);
//	sholderLeftVec = jnt;
//}
//else if (jointToRecord[i] == NUI_SKELETON_POSITION_ELBOW_LEFT)
//{
//	skeletFullWidth += sholderLeftVec.distance(jnt);
//	elbowLeftVec = jnt;
//}
//else if (jointToRecord[i] == NUI_SKELETON_POSITION_WRIST_LEFT)
//{
//	skeletFullWidth += elbowLeftVec.distance(jnt);
//}	