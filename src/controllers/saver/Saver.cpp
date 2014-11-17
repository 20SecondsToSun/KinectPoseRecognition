#include "Saver.h"

using namespace ci;
using namespace ci::app;
using namespace std;

std::vector<Pose*> Saver::loadPoseBase()
{
	std::vector<Pose*>  posesVector;
	fs::path basePath = Params::getBaseStoragePath();

	for (fs::directory_iterator it(basePath); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			JsonTree doc;

			if (it->path().extension() != ".json") 
				continue;

			try{
				doc = JsonTree(loadFile(basePath / it->path().filename().string()));
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
				for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data )
				{
					float x =  data->getChild( "x" ).getValue<float>();
					float y =  data->getChild( "y" ).getValue<float>();
					float z =  data->getChild( "z" ).getValue<float>();
					points.push_back(Vec3f(x, y, z));
				}

				newPose->setPoints(points);
				newPose->createNormalizePoints0();
				newPose->setRedColors();

				string name =  pose->getChild( "name" ).getValue<string>();
				newPose->setName(name);

				int tilt =  pose->getChild( "tilt" ).getValue<int>();
				newPose->setKinectTilt(tilt);

				int time =  pose->getChild( "time" ).getValue<int>();
				newPose->setPoseTime(time);

				int percent =  pose->getChild( "percent" ).getValue<int>();
				newPose->setPercent(percent);

				gl::Texture tex;
				try{
					tex = loadImage(basePath /(name +".png"));			
				}
				catch(...)
				{
					continue;
				}		

				newPose->setImage(tex);

				name =  pose->getChild( "comics" ).getValue<string>();
				try{
					tex = loadImage(basePath / (name +".png"));			
				}
				catch(...)
				{
					continue;
				}	

				newPose->setComicsImage(tex);

				posesVector.push_back(newPose);
			}	
		}
	}

	return posesVector;
}

void Saver::savePoseIntoBase(Pose* pose)
{
	fs::path basePath = Params::getPoseSavingPath();

	JsonTree doc;
	JsonTree posesJson = JsonTree::makeArray( "poses" );

	string poseName = pose->getName();

	JsonTree onePoseJson;
	onePoseJson.addChild( JsonTree( "name", poseName ) );		
	onePoseJson.addChild( JsonTree( "tilt", pose->getKinectTilt()));
	onePoseJson.addChild( JsonTree( "time", pose->getPoseTime()));	
	onePoseJson.addChild( JsonTree( "comics", pose->getComicsName()));
	onePoseJson.addChild( JsonTree( "percent", pose->getPercent()));	

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
	doc.write( writeFile( basePath / jsonName ), JsonTree::WriteOptions() );

	string genericName = poseName + ".png";	
	writeImage(  basePath / genericName, pose->getImage());
}

bool Saver::saveImageIntoBase(string mails,  ci::Surface  image)
{
	bool isSave = true;
	time_t     now = time(0);
	struct tm  tstruct;
	tstruct = *localtime(&now);

	string month = to_string(tstruct.tm_mon+1);
	string day   = to_string(tstruct.tm_mday);

	if (month.size() < 2)
		month = "0"+ month;

	if (day.size() < 2)
		day = "0"+ day;

	string dateName = to_string(1900 + tstruct.tm_year) + month + day;
	string dirname = "data\\photo\\" + to_string(Params::standID) + "." + dateName;
	string fileName = to_string(Params::standID) + "." + dateName + ".csv";
	fs::path dir_path = getAppPath()/dirname;
	fs::path file_path = getAppPath()/dirname/fileName;

	fs::path image_path  ="";

	string hours =  to_string(tstruct.tm_hour)+":"+to_string(tstruct.tm_min)+":"+to_string(tstruct.tm_sec);
	string saveString = hours+";"+mails+"\n";

	if(fs::is_directory(dir_path))
	{
		if (checkFile(file_path, saveString))
		{
			image_path = getAppPath()/dirname/(to_string(getImagesInDir(dir_path))+".jpg");
			writeImage( image_path, image);
		}	
		else
			isSave = false;
	}
	else
	{		
		if(fs::create_directory(dir_path))
		{			
			if (checkFile(file_path, saveString))
			{
				image_path = getAppPath()/dirname/(to_string(getImagesInDir(dir_path))+".jpg");
				writeImage( image_path, image);
			}
			else
				isSave = false;
		}
		else		
			isSave = false;
	}

	return isSave;
}

bool Saver::checkFile(fs::path filepath, string mails)
{
	FILE* file= fopen(filepath.string().c_str(), "a");

	if (file!=NULL)
	{
		fputs ((mails).c_str(),file);
		fclose (file);
		return true;
	}

	return false;
}

int Saver::getImagesInDir(fs::path dir_path)
{			
	int count = 0;

	for (fs::directory_iterator it(dir_path); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
			count++;	
	}

	return count;
}

void Saver::loadConfigData()
{
	fs::path filepath = Params::getConfigStoragePath();

	JsonTree doc;

	try{
		doc = JsonTree(loadFile(filepath));
		Params::standID =  doc.getChild( "standID" ).getValue<int>() ;
		Params::isNetConnected =  doc.getChild( "netConnection" ).getValue<bool>() ;
		Params::isFullSkelet =  doc.getChild( "fullskelet" ).getValue<bool>() ;
		Params::isPointsDraw =  doc.getChild( "drawPoints" ).getValue<bool>() ;
		Params::computeMistakeAlgo =  doc.getChild( "algo" ).getValue<int>() ;

		Params::maxErrorBetweenJoints =  doc.getChild( "maxError" ).getValue<int>() ;
		Params::minErrorBetweenJoints =  doc.getChild( "minError" ).getValue<int>() ;
		Params::etalonHeight          =  doc.getChild( "etalonHeight" ).getValue<int>() ;


		Params::maxUserDistance=  doc.getChild( "maxUserDistance" ).getValue<float>() ;
		Params::minUserDistance =  doc.getChild( "minUserDistance" ).getValue<float>() ;
		Params::maxUserHeight =  doc.getChild( "maxUserHeight" ).getValue<float>() ;
		Params::minUserHeight  =  doc.getChild( "minUserHeight" ).getValue<float>() ;

		JsonTree datas =JsonTree( doc.getChild( "percents" ));

		int i = 0;
		for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data )		
			Params::weightJoints[i++] = data->getValue<float>();

		console()<<"PARAMS :: "<<Params::standID<<" isNetConnected "<< Params::isNetConnected <<" computeMistakeAlgo "<<Params::computeMistakeAlgo<<std::endl;
	}
	catch(...)
	{

	}
}