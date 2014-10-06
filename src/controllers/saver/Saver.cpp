#include "Saver.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace SaverDefaults;

std::vector<Pose*> Saver::loadPoseBase()
{

	std::vector<Pose*>  posesVector;		

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
			
			JsonTree poses( doc.getChild( "poses" ) );

			for( JsonTree::ConstIter pose = poses.begin(); pose != poses.end(); ++pose ) 
			{
				Pose* newPose = new Pose();	
				vector<Vec3f> points;
			
				JsonTree datas =JsonTree( pose->getChild( "data" ));
				for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data ) {		

					float x =  data->getChild( "x" ).getValue<float>();
					float y =  data->getChild( "y" ).getValue<float>();
					float z =  data->getChild( "z" ).getValue<float>();

					points.push_back(Vec3f(x, y, z));
				}

				newPose->setPoints(points);	
				newPose->createBoundingBox();
				newPose->createNormalizePoints();				

				string name =  pose->getChild( "name" ).getValue<string>();
				newPose->setName(name);

				int tilt =  pose->getChild( "tilt" ).getValue<int>();
				newPose->setKinectTilt(tilt);

				gl::Texture tex;
				try{
					tex = loadImage(getAppPath() /JSON_STORAGE_FOLDER/(name +".png"));			
				}
				catch(...)
				{
					continue;
				}

				newPose->setImage(tex);
				posesVector.push_back(newPose);
			}	
		}
	}

	return posesVector;
}

void Saver::savePoseIntoBase(Pose* pose)
{
	JsonTree doc;
	JsonTree posesJson = JsonTree::makeArray( "poses" );

	string poseName = pose->getName();

	JsonTree onePoseJson;
	onePoseJson.addChild( JsonTree( "name", poseName ) );		
	onePoseJson.addChild( JsonTree( "tilt", pose->getKinectTilt()));	

	JsonTree gesturesDataJson = JsonTree::makeArray( "data" );

	auto points		  =  pose->getPoints();
	
	for (size_t j = 0, ilen = points.size() ; j < ilen; j++)
	{
		JsonTree point;
		point.addChild(JsonTree( "x",points[j].x) );	
		point.addChild(JsonTree( "y",points[j].y) );	
		point.addChild(JsonTree( "z",points[j].z) );
		gesturesDataJson.pushBack(point);
	}

	onePoseJson.addChild(gesturesDataJson );
	posesJson.pushBack(onePoseJson);
	doc.pushBack( posesJson );

	string jsonName = poseName + ".json";	
	doc.write( writeFile( getAppPath() / JSON_STORAGE_FOLDER / jsonName ), JsonTree::WriteOptions() );
	
	string genericName = poseName + ".png";	
	writeImage( getAppPath() /JSON_STORAGE_FOLDER/ genericName, pose->getImage());
}