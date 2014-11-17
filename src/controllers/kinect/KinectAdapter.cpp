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
		if (skeleton.size() == 0 ) continue;

		int notTrackedPointsCount = 0;
		
		for (size_t i = 0, len = jointToRecord.size(); i < len; i++)
		{
			auto joint = skeleton.find(jointToRecord[i]);
			if ( joint != skeleton.end())
			{
				const MsKinect::Bone& bone = joint->second;

				//if(NUI_SKELETON_POSITION_TRACKED != bone.getTrackingState())
				//{
				//	notTrackedPointsCount++;
				//	if (notTrackedPointsCount > 4)
				//	{
				//		//currentSkelet.clear();
				//		//break;
				//	}
				//}

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

				if(NUI_SKELETON_POSITION_TRACKED != bone.getTrackingState())
				{
					//rawCurrentSkelet.clear();
					//break;					
				}				
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

		// console()<<" leftFootY "<<leftFootY<<" rightFootY "<<rightFootY<<endl;

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
	if (rawCurrentSkelet.size() == humanGrowthPoints.size())
	{
		float _height = 0.0f;
		_height += rawCurrentSkelet[3].distance(rawCurrentSkelet[2]);
		_height += rawCurrentSkelet[2].distance(rawCurrentSkelet[1]);
		_height += rawCurrentSkelet[2].distance(rawCurrentSkelet[0]);
		_height += rawCurrentSkelet[0].distance(rawCurrentSkelet[4]);
		_height += rawCurrentSkelet[4].distance(rawCurrentSkelet[5]);
		_height += rawCurrentSkelet[5].distance(rawCurrentSkelet[6]);
		//_height += rawCurrentSkelet[6].distance(rawCurrentSkelet[7]);

		return _height;

	//3-2
	//2-1
	//1-0
	//0-4
	//4-5
	//5-6
	//6-7

		//float leftFootY = rawCurrentSkelet[ANKLE_LEFT_INDEX].y;	
		//float rightFootY = rawCurrentSkelet[ANKLE_RIGHT_INDEX].y;	
		//float headY = rawCurrentSkelet[HEAD_INDEX].y;

		//return rawCurrentSkelet[0].distance(rawCurrentSkelet[1]);
		//return abs(headY - leftFootY);
	}
	
	return 0.0f;
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