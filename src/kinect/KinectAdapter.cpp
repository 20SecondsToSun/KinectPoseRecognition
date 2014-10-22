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
	connect();
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

	_isConnected ? bufferDisconnect = 0 :bufferDisconnect++;

	if (bufferDisconnect > 60)
	{
		_isConnected = false;

		if(!reconnectTimer.isStopped())
		{
			if(reconnectTimer.getSeconds()> 5)
			{
				reconnectTimer.stop();
				bufferDisconnect = 0;
				connect();
			}
		}
		else
		{
			reconnectTimer.start();
		}
	}
	else
	{
		//_isConnected = true;
		if (!reconnectTimer.isStopped())	
			reconnectTimer.stop();		
	}
}

void KinectAdapter::updateSkeletonData()
{	
	if (!isTracking || !_isConnected) return;

	currentSkelet.clear();
	
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
	#ifdef debug
		return 1;
	#endif

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
		//drawLoadedPoses();
		drawSkeletJoints();
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
			gl::draw(Texture(savePoseDepth));
		gl::popMatrices();
	}
}