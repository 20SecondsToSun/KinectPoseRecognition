#include "KinectAdapter.h"

using namespace kinectDefaults;

KinectAdapter KinectAdapter::KinectAdapterState;

void KinectAdapter::Setup()
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

	jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_KNEE_RIGHT);
	jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
	 
	jointToRecord.push_back(NUI_SKELETON_POSITION_ANKLE_RIGHT);

	currentPoseInGame  =  0;

	isTracking = true;
	hintFont = ci::Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 26 );

	deviceOptions.enableColor(true);
	deviceOptions.enableDepth(true);
	deviceOptions.setDepthResolution(DEPTH_RESOLUTION);
	deviceOptions.setColorResolution(COLOR_RESOLUTION);	
	deviceOptions.setSkeletonSelectionMode(MsKinect::SkeletonSelectionMode::SkeletonSelectionModeClosest1);

	mDevice = MsKinect::Device::create();	 	

	mDevice->connectEventHandler( [ & ]( MsKinect::Frame frame )
	{
		mFrame = frame;

		if ( frame.getColorSurface() ) 
		{
			mTextureColor = gl::Texture::create( frame.getColorSurface() );
			_mTextureColor = gl::Texture(frame.getColorSurface());
			surface8u =  frame.getColorSurface();
		} 
		else if ( frame.getInfraredChannel() ) 
		{
			mTextureColor = gl::Texture::create( frame.getInfraredChannel() );			
		}
		if ( frame.getDepthChannel() ) 
		{
			mTextureDepth	 = gl::Texture::create( frame.getDepthChannel() );
			mDepthChannel16u = frame.getDepthChannel();
		}
	} );

	calculateAspects();	
	kinectConnect();	
}

void KinectAdapter::kinectConnect()
{
	try {
		mDevice->start(deviceOptions);
	} catch ( MsKinect::Device::ExcDeviceCreate ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcDeviceInit ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcDeviceInvalid ex ) {
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcGetCoordinateMapper ex ) {
	//	console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcOpenStreamColor ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcOpenStreamDepth ex ) {
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcStreamStart ex ) {
	//	console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcUserTrackingEnable ex ) {
	//	console() << ex.what() << endl;
	}
}

void KinectAdapter::loadPoseBase()
{
	string path = getAppPath().string() + JSON_STORAGE_FOLDER;
	fs::path p(path);	

	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			JsonTree doc;
			try{
				 doc = JsonTree(loadFile(getAppPath() / JSON_STORAGE_FOLDER / it->path().filename().string()));
			}
			catch(...)
			{
				continue;
			}
			
		//	JsonTree doc = JsonTree(loadFile(getAppPath() / DATA_BASE_NAME));
			JsonTree poses( doc.getChild( "poses" ) );

			for( JsonTree::ConstIter pose = poses.begin(); pose != poses.end(); ++pose ) 
			{
				Pose* newPose = new Pose();	
				vector<Vec3f> points;
				vector<Vec3f> originPoints;

				JsonTree datas =JsonTree( pose->getChild( "data" ));
				for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data ) {
					float x =  data->getChild( "x" ).getValue<float>();
					float y =  data->getChild( "y" ).getValue<float>();
					float z =  data->getChild( "z" ).getValue<float>();
					points.push_back(Vec3f(x, y, z));

					float xO =  data->getChild( "xO" ).getValue<float>();
					float yO =  data->getChild( "yO" ).getValue<float>();
					float zO =  data->getChild( "zO" ).getValue<float>();

					originPoints.push_back(Vec3f(xO, yO, zO));

				}
				newPose->setPoints(points);	
				newPose->setOriginPoints(originPoints);

				Vec3f boXYZ0;

				boXYZ0.x = pose->getChild( "boxX0").getValue<float>();
				boXYZ0.y = pose->getChild( "boxY0" ).getValue<float>();	
				boXYZ0.z = pose->getChild( "boxZ0").getValue<float>();	

				newPose->setXYZ0(boXYZ0);

				string name =  pose->getChild( "name" ).getValue<string>();
				newPose->setName(name);

				gl::Texture tex;
				try{
					tex = loadImage(getAppPath() /JSON_STORAGE_FOLDER/(name +".png"));
					//console()<<"----------------------------"<<name<<endl;
				}
				catch(...)
				{
					continue;
				}
				newPose->setImage(tex);

				addPose(newPose);
			}	
		}
	}
}

void KinectAdapter::addPose(Pose* pose)
{
	poses.push_back(pose);
}

void KinectAdapter::savePoseBase()
{
	JsonTree doc;
	JsonTree posesJson = JsonTree::makeArray( "poses" );

	string __name = "empty";

	size_t i = poses.size()-1;
	//for (size_t i = 0, ilen = poses.size(); i<ilen; i++)
	//{
		__name = poses[i]->getName();

		JsonTree onePoseJson;
		onePoseJson.addChild( JsonTree( "name", __name ) );		

		JsonTree gesturesDataJson = JsonTree::makeArray( "data" );

		auto points		  =  poses[i]->getPoints();
		auto originPoints =  poses[i]->getOriginPoints();

		for (size_t j = 0, ilen = points.size() ; j < ilen; j++)
		{
			JsonTree point;
			point.addChild(JsonTree( "x",points[j].x) );	
			point.addChild(JsonTree( "y",points[j].y) );	
			point.addChild(JsonTree( "z",points[j].z) );

			point.addChild(JsonTree( "xO",originPoints[j].x) );	
			point.addChild(JsonTree( "yO",originPoints[j].y) );	
			point.addChild(JsonTree( "zO",originPoints[j].z) );
			
			gesturesDataJson.pushBack(point);
		}

		Vec3f boXYZ0 = poses[i]->getXYZ0();

		onePoseJson.addChild( JsonTree( "boxX0", boXYZ0.x ) );	
		onePoseJson.addChild( JsonTree( "boxY0", boXYZ0.y ) );	
		onePoseJson.addChild( JsonTree( "boxZ0", boXYZ0.z ) );	

		onePoseJson.addChild(gesturesDataJson );

		posesJson.pushBack(onePoseJson);
	//}

	doc.pushBack( posesJson );

	string jsonName = __name + ".json";	
	doc.write( writeFile( getAppPath() / JSON_STORAGE_FOLDER / jsonName ), JsonTree::WriteOptions() );
	
	Surface16u surf = MsKinect::greenScreenUsers(mDepthChannel16u, surface8u, COLOR_RESOLUTION, DEPTH_RESOLUTION); 
	string genericName = __name + ".png";	
	writeImage( getAppPath() /JSON_STORAGE_FOLDER/ genericName, savePoseDepth);
}

void KinectAdapter::update() 
{
	updateSkeletonData();

	if (gameMode == "MatchingGame" && isGameRunning)	
	{
		matchTemplate();

		if (mathPercent > Params::percentForMatching ) levelCompletion +=2;
		else levelCompletion = 0;
	}
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




void KinectAdapter::drawPoseComics() 
{

}

void KinectAdapter::updateSkeletonData()
{
	if (isTracking == false) return;
	
	currentSkelet.clear();
	currentSkeletToDraw.clear();


	for ( const auto& skeleton : mFrame.getSkeletons() ) 
	{
		
		for (int i = 0; i < jointToRecord.size(); i++)
		{
			if(skeleton.find(jointToRecord[i]) != skeleton.end())
			 {
				const MsKinect::Bone& bone = skeleton.find(jointToRecord[i])->second;

				Vec2f v0				   = mDevice->mapSkeletonCoordToDepth( bone.getPosition() );
				auto  z					   = skeleton.at( bone.getStartJoint()).getPosition().z;			
			//	console()<<"z::::::::::::  "<<z<<endl;
				currentSkelet.push_back(Vec3f(v0.x, v0.y, z));
				currentSkeletToDraw.push_back(Vec3f(v0.x, v0.y, z));
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
	//console()<<" game mode  "<<gameMode<<endl;

	if (gameMode == "RecordingPose")
	{	
		drawUserMask();
		drawSkeletJoints();
	}
	else if (gameMode == "MatchingGame")
	{
		drawLoadedPoses();
		drawSkeletJoints();
	}
}

void KinectAdapter::drawLoadedPoses()
{
	gl::enableAlphaBlending();
	gl::pushMatrices();
		gl::translate(viewShiftX, viewShiftY);
		gl::scale(headScale, headScale);
		gl::color(ColorA(1,0,0,0.8));
		gl::draw(poses[currentPoseInGame]->getImage());
	gl::popMatrices();

	//gl::color(Color::white());
	//Utils::textFieldDraw(to_string(levelCompletion), hintFont, Vec2f(100.f, 260.0f), Color::hex(0x000000));
}

void KinectAdapter::drawSkeletJoints()
{
	gl::pushMatrices();
		gl::translate(viewShiftX, viewShiftY);
		gl::scale(headScale, headScale);

		for (int i = 0; i < currentSkeletToDraw.size(); i++)		
			gl::drawSolidCircle( Vec2f(currentSkeletToDraw[i].x, currentSkeletToDraw[i].y), 5.0f, 16 );		
			
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

void KinectAdapter::swapTracking()
{
	isTracking = !isTracking;
}

void KinectAdapter::stopTracking()
{
	isTracking = false;
}

void KinectAdapter::startTracking()
{
	isTracking = true;
}

void KinectAdapter::saveAsTemplate(string name)
{
	if (!currentSkelet.size()) return;

	Pose* pose = new Pose();
	pose->setName(name);
	pose->setOriginPoints(currentSkelet);

	preparePoseTemplate();	

	pose->setPoints(currentSkelet);
	pose->setXYZ0(x0y0z0BoxPoint);
	pose->setImage(Texture(savePoseDepth));
	addPose(pose);

	savePoseBase();
}

void KinectAdapter::matchTemplate()
{
	preparePoseTemplate();

	double min_dist			 = FLT_MAX;
	foundPose				 = NULL;
	double maxErrorToDiscard = 1000;

	if(poses[currentPoseInGame]->getPoints().size() != currentSkelet.size())
	{
		mathPercent = 0;
		return;
	}	

	if (abs(poses[currentPoseInGame]->getXYZ0().x - x0y0z0BoxPoint.x) > Params::boxMaxErrorX || abs(poses[currentPoseInGame]->getXYZ0().y -x0y0z0BoxPoint.y) > Params::boxMaxErrorY  )
	{
		mathPercent = 0;
		return;
	}

	double d		 = 0;		
	bool big_mistake = false;

	for(int j = 0; j < currentSkelet.size(); ++j) 
	{
		float mistake = (Vec2f(currentSkelet[j].x, currentSkelet[j].y) - Vec2f(poses[currentPoseInGame]->getPoints()[j].x, poses[currentPoseInGame]->getPoints()[j].y) ).length();
		//float mistake =  (currentSkelet[j] - poses[i]->getPoints()[j]).length();
		d += mistake;

		if (mistake > Params::maxErrorBetweenJoints) 
		{
			big_mistake = true;
			break;
		}
	}

	//console()<<"=================  big_mistake =============="<<endl;

	if (big_mistake) 
	{
		mathPercent = 0;
		return;
	}

	double dist = d/currentSkelet.size();

	if(dist > 0 && dist < min_dist) 
	{
		min_dist = dist;
		foundPose = poses[currentPoseInGame];
	}

	double half_diagonal = 0.5 * sqrt((BOX_SCALE*BOX_SCALE) + (BOX_SCALE*BOX_SCALE));//+ (BOX_SCALE*BOX_SCALE));	 
	mathPercent = 1.0 - (min_dist/half_diagonal);	
}

void KinectAdapter::preparePoseTemplate()
{
	setX0Y0Z0BoxPoint();
	scaleTo();
	translateTo();
}

void KinectAdapter::setX0Y0Z0BoxPoint()
{
	boxStruct rect = boundingBox();

	x0y0z0BoxPoint.x = rect.x;
	x0y0z0BoxPoint.y = rect.y;
	x0y0z0BoxPoint.z = rect.z;	
}

void KinectAdapter::scaleTo()
{
	boxStruct rect = boundingBox();
	
	for (int i = 0; i < currentSkelet.size(); i++)
	{
		Vec3f* v = &(currentSkelet[i]);
		v->x = v->x * (BOX_SCALE/(/*rect.x -*/ rect.w));
		v->y = v->y * (BOX_SCALE/(/*rect.y -*/ rect.h));	
		v->z = v->z * (BOX_SCALE/(/*rect.z -*/ rect.d));
	}
}

void KinectAdapter::translateTo()
{
	calculateCentroid();

	for (int i = 0; i < currentSkelet.size(); i++)
	{
		Vec3f* v = &(currentSkelet[i]);
		v->x = v->x - centroid.x + BOX_SCALE*0.5;
		v->y = v->y - centroid.y + BOX_SCALE*0.5;
		v->z = v->z - centroid.z + BOX_SCALE*0.5;
	}
}

boxStruct KinectAdapter::boundingBox() 
{
	double min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN, min_z = FLT_MAX, max_z = FLT_MIN;
	
	for (int i = 0; i < currentSkelet.size(); i++)
	{

		Vec3f v = currentSkelet[i];
		if(v.x < min_x) min_x = v.x;
		if(v.x > max_x) max_x = v.x;
		if(v.y < min_y) min_y = v.y;
		if(v.y > max_y) max_y = v.y;	
		if(v.z < min_z) min_z = v.z;
		if(v.z > max_z) max_z = v.z;
	}

	boxStruct rect;
	rect.x = min_x;
	rect.y = min_y;
	rect.z = min_z;
	
	rect.w = (max_x - min_x);
	rect.h = (max_y - min_y);
	rect.d = (max_z - min_z);

	return rect;
}

void KinectAdapter::calculateCentroid()
{
	double x = 0;
	double y = 0;
	double z = 0;

	for (int i = 0; i < currentSkelet.size(); i++)
	{
		x += currentSkelet[i].x;
		y += currentSkelet[i].y;
		z += currentSkelet[i].z;
	}

	x /= currentSkelet.size();
	y /= currentSkelet.size();
	z /= currentSkelet.size();

	
	centroid = Vec3f(x, y, z);	
}

void KinectAdapter::Shutdown()
{	
	mDevice->stop();	
}


ci::gl::TextureRef KinectAdapter::getColorTexRef()
{
	return mTextureColor;
}
ci::gl::Texture KinectAdapter::getColorTex()
{
	return _mTextureColor;
}
ci::gl::TextureRef KinectAdapter::getDepthTexRef()
{
	return mTextureDepth;
}
ci::Surface8u  KinectAdapter::getSurface8u()
{
	return surface8u;
}
Rectf KinectAdapter::getColorResolutionRectf()
{
	switch (COLOR_RESOLUTION)
	{
		case MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_640x480 :
			return Rectf(0.0f, 0.0f, 640.0f,480.0f);
		break;

		case MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_1280x960 :
			return Rectf(0.0f, 0.0f, 1280.0f,960.0f);
		break;
	}		
}	
void KinectAdapter::drawKinectCameraColorSurface()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize());
	//gl::enableAlphaBlending();

	//console()<<"drawKinectCameraColorSurface"<<getElapsedFrames()<<endl;

	Texture colorRef =  getColorTex();

	if (colorRef)
	{
		console()<<"colorRef"<<getElapsedFrames()<<endl;
		gl::enable( GL_TEXTURE_2D );	

		gl::color( ColorAf::white() );
		
		gl::pushMatrices();
			gl::translate(viewShiftX, viewShiftY);	
			gl::draw( colorRef, colorRef.getBounds(), Rectf(0, 0, float(viewWidth),float(viewHeight)));	
		gl::popMatrices();		

		gl::disable( GL_TEXTURE_2D );
	}

	//gl::disableAlphaBlending();
}
void KinectAdapter::calculateAspects()
{
	Rectf kinectResolutionR = getColorResolutionRectf();
	float aspect =  kinectResolutionR.getWidth()/kinectResolutionR.getHeight();
			
	if( getWindowWidth() / getWindowHeight() > aspect)			
	{
		viewHeight = getWindowHeight();
		viewWidth = int(viewHeight * aspect);	
		headScale = viewHeight/ kinectResolutionR.getHeight();
	}
	else 
	{ 
		viewWidth = getWindowWidth();
		viewHeight = int(viewWidth / aspect);	
		headScale  = viewWidth/ kinectResolutionR.getWidth();
	}
	
	viewShiftX =float( 0.5 * (getWindowWidth()  - viewWidth));
	viewShiftY= float( 0.5 * (getWindowHeight() - viewHeight));		
}
