#include "KinectAdapter.h"

using namespace kinectDefaults;
using namespace HumanGrowth;

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


	jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_RIGHT);	
	jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_RIGHT);

	SKELET_SIZE = 12;//jointToRecord.size();

	HEAD_INDEX = 1;
	ANKLE_RIGHT_INDEX = 11;
	ANKLE_LEFT_INDEX = 9;
	WRIST_RIGHT_INDEX = 7;
	WRIST_LEFT_INDEX = 4;

	SCREEN_HEIGHT = 1080.0f;

	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_SPINE);
	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_HEAD);
	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_HIP_LEFT);
	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
	humanGrowthPoints.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
	//humanGrowthPoints.push_back(NUI_SKELETON_POSITION_FOOT_LEFT);
}

void KinectAdapter::enbleGestures()
{
	_gestureEnable = true;
	_handsUpGestureSignal = _handsUpGesture.gestureRecognizedEvent.connect( [ & ]( )
	{
		gestureEvent();
	});	
}

void KinectAdapter::disableGestures()
{
	_gestureEnable = false;
	if (_handsUpGestureSignal.connected())
		_handsUpGestureSignal.disconnect();
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
	rawCurrentSkelet.clear();

	for ( const auto& skeleton : mFrame.getSkeletons() ) 
	{
		if (skeleton.size() == 0 )
			continue;

		int notTrackedPointsCount = 0;

		for (size_t i = 0, len = jointToRecord.size(); i < len; i++)
		{
			auto joint = skeleton.find(jointToRecord[i]);
			if ( joint != skeleton.end())
			{
				const MsKinect::Bone& bone = joint->second;

				//if(NUI_SKELETON_POSITION_TRACKED != bone.getTrackingState())
				Vec2f v0 = mDevice->mapSkeletonCoordToColor( bone.getPosition());
				auto  z = skeleton.at( bone.getStartJoint()).getPosition().z;
				currentSkelet.push_back( Vec3f(v0.x, v0.y, z));
			}
		}

		for (size_t i = 0, len = humanGrowthPoints.size(); i < len; i++)
		{
			auto joint = skeleton.find(humanGrowthPoints[i]);
			if ( joint != skeleton.end())
			{
				const MsKinect::Bone& bone = joint->second;		
				rawCurrentSkelet.push_back( bone.getPosition());
			}
		}
		if (_gestureEnable)
		{
			_handsUpGesture.update(skeleton);
		}
		break;
	}

	if(getSurface8u() && getDepthChannel16u() && currentSkelet.size()!=0 && mDevice->isCapturing())	
		savePoseDepth = MsKinect::greenScreenUsers(getDepthChannel16u(), getSurface8u(), COLOR_RESOLUTION, DEPTH_RESOLUTION);	
}

void KinectAdapter::updateSkeletonData(Rectf trackZone)
{	
	if (!isTracking || !_isConnected) return;

	vector<SkeletJoints> colorMapSkelets, rawDataSkelets;
	vector<Vec3f> spinePositions;
	vector<MsKinect::Skeleton> skeletons;

	for ( const auto& skeleton : mFrame.getSkeletons() ) 
	{
		currentSkelet.clear();
		rawCurrentSkelet.clear();

		if (skeleton.size() == 0 )
			continue;

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

		for (size_t i = 0, len = humanGrowthPoints.size(); i < len; i++)
		{
			auto joint = skeleton.find(humanGrowthPoints[i]);
			if ( joint != skeleton.end())
			{
				const MsKinect::Bone& bone = joint->second;		
				rawCurrentSkelet.push_back( bone.getPosition());

				if (humanGrowthPoints[i] == NUI_SKELETON_POSITION_SPINE)
				{
					Vec2f v0 = mDevice->mapSkeletonCoordToColor( bone.getPosition());
					auto  z = skeleton.at( bone.getStartJoint()).getPosition().z;
					spinePositions.push_back(Vec3f(v0.x, v0.y, z));
				}
			}
		}

		colorMapSkelets.push_back(currentSkelet);
		rawDataSkelets.push_back(rawCurrentSkelet);
		skeletons.push_back(skeleton);
	}

	currentSkelet.clear();
	rawCurrentSkelet.clear();
	//

	int setType = -1;

	if (skeletons.size() == 2)
	{
		Vec2f spineVec2f0 =  headScale *Vec2f(spinePositions[0].x, spinePositions[0].y) + Vec2f(viewShiftX, viewShiftY);
		Vec2f spineVec2f1 =  headScale * Vec2f(spinePositions[1].x, spinePositions[1].y) + Vec2f(viewShiftX, viewShiftY);

		if (trackZone.contains(spineVec2f0) && trackZone.contains(spineVec2f1))
		{
			if (spinePositions[0].z > spinePositions[1].z)
				setType = 1;
			else
				setType = 0;
		}
		else if (trackZone.contains(spineVec2f0))
		{
			setType = 0;
		}
		else if (trackZone.contains(spineVec2f1))
		{
			setType = 1;
		}
	}
	else if (skeletons.size() == 1)
	{
		Vec2f spineVec2f =  headScale * Vec2f(spinePositions[0].x, spinePositions[0].y) + Vec2f(viewShiftX, viewShiftY);
		if (trackZone.contains(spineVec2f))	
		{
			setType = 0;	
		}
	}

	if (setType == 0)
	{
		currentSkelet = colorMapSkelets[0];
		rawCurrentSkelet = rawDataSkelets[0];
		if (_gestureEnable)	
			_handsUpGesture.update( skeletons[0]);
	}
	else if (setType == 1)
	{
		currentSkelet = colorMapSkelets[1];
		rawCurrentSkelet = rawDataSkelets[1];
		if (_gestureEnable)	
			_handsUpGesture.update( skeletons[1]);	
	}
	else
	{
		if (_gestureEnable)	
			_handsUpGesture.reset();
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

	return peopleInFrame;
}

bool KinectAdapter::allHumanPointsInScreenRect() 
{
	if (currentSkelet.size() == SKELET_SIZE)
	{
		Vec3f head = currentSkelet[HEAD_INDEX];
		float posHeadY = headScale * head.y + viewShiftY;

		Vec3f leftFoot = currentSkelet[ANKLE_LEFT_INDEX];
		Vec3f rightFoot = currentSkelet[ANKLE_RIGHT_INDEX];

		float leftFootY = headScale * leftFoot.y + viewShiftY;
		float rightFootY = headScale * rightFoot.y + viewShiftY;

		if (posHeadY > 0.0f && leftFootY <= SCREEN_HEIGHT && rightFootY <= SCREEN_HEIGHT)
			return true;
	}

	return false;
}

float KinectAdapter::calculateHumanGrowsScaleTo180sm() 
{
	SkeletJoints skelet = getCurrentSkelet();

	float  userHeight = userHeightInPixels();
	float scaleAccordingUserHeight;
	float etalonHeightAccordingDepth = getEtalonHeightInPixelsAccordingDepth();

	if(etalonHeightAccordingDepth)
		scaleAccordingUserHeight =  userHeight / etalonHeightAccordingDepth;
	else scaleAccordingUserHeight = 1;

	scaleAccordingUserHeight = Utils::clamp(scaleAccordingUserHeight, 1.0f, 0.5f);

	return scaleAccordingUserHeight;
}

float KinectAdapter::getEtalonHeightInPixelsAccordingDepth()
{
	float _distance = distanceToSkelet();

	if (_distance)
	{
		float x1 = MAX_USER_HEIGHT;
		float y1 = MIN_USER_DISTANCE;

		float x2 = MIN_USER_HEIGHT;
		float y2 = MAX_USER_DISTANCE;

		float del = (y2 - y1);
		float etalonHeightAccordingDepth;

		_distance = Utils::clamp(_distance, MAX_USER_DISTANCE, MIN_USER_DISTANCE);

		if (del)
		{
			etalonHeightAccordingDepth = ((x1 * y2 - x2 * y1) + (x2 - x1) * _distance) / del;
		}
		else
		{
			etalonHeightAccordingDepth = MAX_USER_HEIGHT;
		}

		return etalonHeightAccordingDepth;
	}

	return 0.0f;
}

float KinectAdapter::userHeightInPixels()
{
	if (currentSkelet.size() == SKELET_SIZE)
	{		
		float leftFootY = currentSkelet[ANKLE_LEFT_INDEX].y;	
		float rightFootY = currentSkelet[ANKLE_RIGHT_INDEX].y;	
		float headY = currentSkelet[HEAD_INDEX].y;
		return abs(headY - leftFootY);
	}

	return 0.0f;
}

float KinectAdapter::userHeightRaw()
{
	float _height = 0.0f;

	if (rawCurrentSkelet.size() == humanGrowthPoints.size())
	{		
		_height += rawCurrentSkelet[3].distance(rawCurrentSkelet[2]);
		_height += rawCurrentSkelet[2].distance(rawCurrentSkelet[1]);
		_height += rawCurrentSkelet[2].distance(rawCurrentSkelet[0]);
		_height += rawCurrentSkelet[0].distance(rawCurrentSkelet[4]);
		_height += rawCurrentSkelet[4].distance(rawCurrentSkelet[5]);
		_height += rawCurrentSkelet[5].distance(rawCurrentSkelet[6]);
	}

	return _height;
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
}

void KinectAdapter::drawSkeletJoints()
{
	gl::pushMatrices();
	gl::translate(viewShiftX, viewShiftY);
	gl::scale(headScale, headScale);
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

bool KinectAdapter::isHandsUp() 
{
	kinect().updateSkeletonData();
	std::vector<ci::Vec3f> skelet = kinect().getCurrentSkelet();

	if (skelet.size() == SKELET_SIZE)
	{
		Vec3f headPosition = skelet[HEAD_INDEX];
		Vec3f rightHand = skelet[WRIST_RIGHT_INDEX];
		Vec3f leftHand = skelet[WRIST_LEFT_INDEX];

		if (leftHand.y < headPosition.y || rightHand.y < headPosition.y)
			return true;
	}
	return false;
}

bool KinectAdapter::isHeapInRect( Rectf rect) 
{
	if (rawCurrentSkelet.size())
	{
		Vec2f v0 = mDevice->mapSkeletonCoordToColor(rawCurrentSkelet[0]);
		return rect.contains(v0);
	}

	return false;
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

bool KinectAdapter::isDistanceOk()
{
	return kinect().distanceToSkelet() > MIN_DISTANCE_TO_SKELET && kinect().distanceToSkelet() < MAX_DISTANCE_TO_SKELET;
}