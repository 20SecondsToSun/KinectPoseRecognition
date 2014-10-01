#include "Dollar.h"
using namespace std;
using namespace ci;
using namespace ci::app;
using namespace dollarDefaults;

Dollar::Dollar() :num_samples(64) ,square_size(250.0) ,angle_precision(1.0) , isRecording(false), isDrawDataBaseGestures(false), currentState("Empty state")
{	 
	 half_diagonal = 0.5 * sqrt((square_size*square_size) + (square_size*square_size));	 
}

void Dollar::loadGestureBase(string& filePath)
{
	
}

void Dollar::loadGestureBase()
{
	hintFont = ci::Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 26 );

	JsonTree doc = JsonTree(loadFile(getAppPath() / DATA_BASE_NAME));
	JsonTree gestures( doc.getChild( "gestures" ) );

	for( JsonTree::ConstIter gesture = gestures.begin(); gesture != gestures.end(); ++gesture ) 
	{
		Gesture* newGesture = new Gesture();	
		vector<Vec3f> points;

		JsonTree datas =JsonTree( gesture->getChild( "data" ));
		for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data ) {
			float x =  data->getChild( "x" ).getValue<float>();
			float y =  data->getChild( "y" ).getValue<float>();

			points.push_back(Vec3f(x,y,0));
		}
		newGesture->setPoints(points);		
		newGesture->setAnglePrecision(angle_precision);

		string name =  gesture->getChild( "name" ).getValue<string>();
		newGesture->setName(name);	
		
		addGesture(newGesture);

		if (uniqueGesturesInBase.find(name) == uniqueGesturesInBase.end())
		{
			uniqueGesturesInBase.insert ( std::pair<std::string, Gesture*>(name, newGesture) );
		}
	}	
}

void Dollar::draw()
{
	if (isDrawDataBaseGestures)
		drawDataBaseGestures();

	gl::color(Color::white());
	for (int i = 0; i < rawData.size(); i++)
	{
		gl::drawSolidCircle(Vec2f(rawData[i].x, rawData[i].y), 5);
	}

	drawStatusBar();
}

void Dollar::drawDataBaseGestures()
{
	float shift  = 5;
	int i = 0;
	gl::color(Color::hex(0xff0000));
	for (auto gesture = uniqueGesturesInBase.begin(); gesture!=uniqueGesturesInBase.end(); gesture++)
	{		
		for (int j = 0; j < (*gesture).second->resampled_points.size(); j++)
		{
			gl::pushMatrices();
			gl::translate(i*(BOX_SCALE+shift), 0 );
			gl::drawSolidCircle(Vec2f(BOX_SCALE*0.5 ,BOX_SCALE*0.5 ) + Vec2f((*gesture).second->resampled_points[j].x,(*gesture).second->resampled_points[j].y), 3);	
			gl::popMatrices();
		}
		i++;
	}
}

void Dollar::drawStatusBar()
{
	gl::enableAlphaBlending();

	ColorA colorZhint = ColorA(1,0,0,1);
	Utils::textFieldDraw(currentState, &hintFont, Vec2f(10.f, getWindowHeight()-50.0f), colorZhint);
}





void Dollar::addGesture(Gesture* gesture)
{
	gestures.push_back(gesture);
}

void Dollar::saveGestureToBase()
{
	JsonTree doc;
	JsonTree gesturesJson = JsonTree::makeArray( "gestures" );

	for (size_t i = 0, ilen= gestures.size(); i<ilen; i++)
	{
		JsonTree oneGestureJson;
		oneGestureJson.addChild( JsonTree( "name", gestures[i]->getName() ) );

		JsonTree gesturesDataJson = JsonTree::makeArray( "data" );

		auto points =  gestures[i]->getPoints();
		for (size_t j = 0, ilen = points.size() ; j < ilen; j++)
		{
			JsonTree point;
			point.addChild(JsonTree( "x",points[j].x) );	
			point.addChild(JsonTree( "y",points[j].y) );	
			gesturesDataJson.pushBack(point);
		}
		oneGestureJson.addChild(gesturesDataJson );

		gesturesJson.pushBack(oneGestureJson);
	}

	doc.pushBack( gesturesJson );

	doc.write( writeFile( getAppPath() / DATA_BASE_NAME ), JsonTree::WriteOptions() );	
}

void Dollar::startRecord()
{
	isRecording = !isRecording;

	if (isRecording)
	{
		rawData.clear();
		currentState = "Recording";
	}
	else
	{	
		currentState = "Try to recognize / Add to base / Clear (q/w/e)";
		/*Gesture* gesture = new Gesture();

		// finalize and add.
		gesture->angle_precision = angle_precision;
		gesture->setPoints(rawData);
		gesture->normalize(num_samples);
		gesture->setName("PIG TAIL");

		//addGesture(gesture);
		//saveGestureToBase();		
		//return;

		double score = -100;
		matchGesture(gesture, &score);

		console()<<" score  ===   "<< score<<endl;*/

		//findInGestures(rawData);
	}
}

void Dollar::execute(char symbol)
{
	if (symbol == 'q')
	{	
		Gesture* gesture = new Gesture();
		gesture->angle_precision = angle_precision;
		gesture->setPoints(rawData);
		gesture->normalize(num_samples);
		double score = -100;
		Gesture* foundGesture =  matchGesture(gesture, &score);
		if (foundGesture != NULL)
			currentState = "name:  " + foundGesture->getName() + "  --- score:  " + to_string(score);
		else 
			currentState = " nothing match .. (";
	}
	else if (symbol == 'w')
	{
		string gestureName = "SQUARE";
		Gesture* gesture = new Gesture();
		gesture->angle_precision = angle_precision;
		gesture->setPoints(rawData);
		gesture->normalize(num_samples);
		gesture->setName(gestureName);
		addGesture(gesture);
		saveGestureToBase();	

		if (uniqueGesturesInBase.find(gestureName) == uniqueGesturesInBase.end())
		{
			uniqueGesturesInBase.insert ( std::pair<std::string, Gesture*>(gestureName, gesture) );
		}

		currentState = "Saved";
	}
	else if (symbol == 'e')
	{
		currentState = "Empty state";
	}
}

Gesture* Dollar::matchGesture(Gesture* gesture, double *pScore)
{
	double min_dist = FLT_MAX;
	Gesture* found_gesture = NULL;

	auto it = gestures.begin();
	while(it != gestures.end()) {
		double dist = (*it)->distanceAtBestAngle(gesture);

		if(dist > 0 && dist < min_dist) 
		{
			min_dist = dist;
			found_gesture = (*it);
		}
		++it;
	}
	*pScore = 1.0 - (min_dist/half_diagonal);
	return found_gesture;
}
void Dollar::stopRecord()
{
	isRecording = false;
}

void Dollar::recording(ci::Vec3f point)
{
	if (isRecording)
		rawData.push_back(point);
}

void Gesture::normalize(int num_samples)
{
	resample(num_samples);

	rotateToZero();

	scaleTo();

	translate();
}

void Gesture::resample(int num_samples)
{
	resampled_points.clear();

	double I = length()/(num_samples-1);
	double D = 0;

	for (int i = 1; i < rawPoints.size(); i++)
	{
		Vec3f	curr = rawPoints[i];
		Vec3f	prev = rawPoints[i-1];
		double	d  = (prev - curr).length();

		if ((D+d) >= I )
		{
			float qx = prev.x + ((I-D)/d) * (curr.x - prev.x);
			float qy = prev.y + ((I-D)/d) * (curr.y - prev.y);
			float qz = prev.z + ((I-D)/d) * (curr.z - prev.z);

			Vec3f resampled(qx, qy, qz);
			resampled_points.push_back(resampled);
			rawPoints.insert(rawPoints.begin() + i, resampled);
			D = 0.0;
		}
		else
			D += d;
	}

	while(resampled_points.size() <= (num_samples - 1)) {
		resampled_points.push_back(rawPoints.back());
	}

	if(resampled_points.size() >num_samples) {
		resampled_points.erase(resampled_points.begin(), resampled_points.begin() + num_samples);	
	}
}

void Gesture::rotateToZero()
{
	calculateCentroid();

	double angle = indicativeAngle();
	resampled_points = rotateBy(resampled_points, -angle);
}

double Gesture::indicativeAngle()
{
	double angle = atan2(centroid.y - resampled_points[0].y, centroid.x - resampled_points[0].x) - 3.1415;//, centroid.z - resampled_points[0].z);
	return angle;
}

vector<Vec3f> Gesture::rotateBy(vector<Vec3f> oPoints, double angle)
{
	rotated_points.clear();
	double cosa = cos(angle);
	double sina = sin(angle);
	vector<Vec3f>::iterator it = oPoints.begin();

	while(it != oPoints.end()) {
		Vec3f v = *it;
		double dx = v.x - centroid.x;
		double dy = v.y - centroid.y;
		v.x = dx * cosa - dy * sina + centroid.x;
		v.y = dx * sina + dy * cosa + centroid.y;
		rotated_points.push_back(v);
		++it;
	}

	return rotated_points;
}

void Gesture::calculateCentroid()
{
	double x = 0;
	double y = 0;
	double z = 0;

	std::vector<Vec3f>::iterator it = resampled_points.begin();

	while(it != resampled_points.end()) {
		x += (*it).x;
		y += (*it).y;
		z += (*it).z;
		++it;
	}

	x /= resampled_points.size();
	y /= resampled_points.size();
	z /= resampled_points.size();

	centroid = Vec3f(x, y, z);	
}


void Gesture::scaleTo(double nSize)
{
	Rectf rect = boundingBox();

	std::vector<Vec3f>::iterator it = resampled_points.begin();

	while(it != resampled_points.end()) {
		Vec3f* v = &(*it);
		v->x = v->x * (nSize/rect.x2);
		v->y = v->y * (nSize/rect.y2);
		++it;
	};
}

void Gesture::translate()
{
	calculateCentroid();

	std::vector<Vec3f>::iterator it = resampled_points.begin();
	while(it != resampled_points.end())
	{
		Vec3f* v = &(*it);
		v->x = v->x - centroid.x;
		v->y = v->y - centroid.y;
		v->z = v->z - centroid.z;
		++it;
	};
}

Rectf Gesture::boundingBox() 
{
	double min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN, min_z = FLT_MIN, max_z = FLT_MAX;
	std::vector<Vec3f>::const_iterator it = resampled_points.begin();
	while(it != resampled_points.end()) {
		Vec3f v = (*it);
		if(v.x < min_x) min_x = v.x;
		if(v.x > max_x) max_x = v.x;
		if(v.y < min_y) min_y = v.y;
		if(v.y > max_y) max_y = v.y;
		//if(v.z < min_z) min_z = v.z;
		//if(v.z > max_z) max_z = v.z;
		++it;
	}
	Rectf rect;
	rect.x1 = min_x;
	rect.y1 = min_y;
	rect.x2 = (max_x - min_x);
	rect.y2 = (max_y - min_y);
	return rect;
}

double Gesture::distanceAtBestAngle(Gesture* pGesture) {

	double angle_range = PI;
	double start_range = -angle_range;
	double end_range = angle_range;
	double x1 = golden_ratio * start_range + (1.0 - golden_ratio) * end_range;
	double f1 = distanceAtAngle(x1, pGesture);
	double x2 = (1.0 - golden_ratio) * start_range + golden_ratio * end_range;
	double f2 = distanceAtAngle(x2, pGesture);

	while(abs(end_range - start_range) > angle_precision)
	{	
		if(f1 < f2)
		{
			end_range = x2;
			x2 = x1;
			f2 = f1;
			x1 = golden_ratio * start_range + (1.0 - golden_ratio) * end_range;
			f1 = distanceAtAngle(x1, pGesture);
		}
		else
		{
			start_range = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1.0 - golden_ratio) * start_range + golden_ratio * end_range;
			f2 = distanceAtAngle(x2, pGesture);
		}

		bool f = abs(end_range - start_range) > angle_precision;	
	}

	return min(f1, f2);
}

double Gesture::distanceAtAngle(double nAngle, Gesture* pGesture)
{	
	vector<Vec3f> points_tmp = resampled_points;	
	points_tmp = rotateBy(points_tmp, nAngle);
	return pathDistance(points_tmp, pGesture);
}

// distance between two paths.
double Gesture::pathDistance(vector<Vec3f> oPoints, Gesture* pGesture) 
{
	// sizes are not equal (?)
	if(oPoints.size() != pGesture->resampled_points.size()) {
		return -1.0;
	}

	double d = 0;

	for(int i = 0; i < resampled_points.size(); ++i) {
		d += (oPoints[i] - pGesture->resampled_points[i]).length();
	}

	return d/oPoints.size();
}

double Gesture::length() 
{
	double len = 0;

	for(int i = 1; i < rawPoints.size(); ++i) {
		len += (rawPoints[i-1] - rawPoints[i]).length();
	}
	return len;
}