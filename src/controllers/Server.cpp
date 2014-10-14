#include "Server.h"

using namespace mndl::curl;
using namespace std;
using namespace ci;

void   Server::sendPhoto(fs::path _path)
{
	photoPath =_path;
	serverThread = shared_ptr<thread>( new thread( bind( &Server::sendToServerImage, this ) ) );	
}

void Server::sendToServerImage( )
{	
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	//writeImage( getAppPath() /FaceController::FACE_STORAGE_FOLDER/serverParams::savePhotoName, ScreenshotHolder::screenshotSmall );
	DataSourceRef urlRequest =	loadFile( photoPath);//getAppPath()/FaceController::FACE_STORAGE_FOLDER/ fs::path( serverParams::savePhotoName ));	
	string loadingString =  toBase64(Buffer(urlRequest)) + to_string(638) +",1";

	ci::app::console()<<".............  "<<loadingString<<endl;

	string jSonstr			 =  Curl::postImage(  serverParams::serverURL, loadingString);
	
	 if (jSonstr.length()>=1)
	 {
		JsonTree jTree;
		try 
		{
			jTree = JsonTree(jSonstr);
			bool success = jTree.getChild("success").getValue<bool>();
		
			if (success==true)
			{
				ci::app::console()<<"SERVER SEND ALL FUCKING DATA  "<<jSonstr<< std::endl;
				link	= jTree.getChild("link").getValue();
				buffer	 = jTree.getChild("data").getValue();

				sessionId = jTree.getChild("id").getValue();
				
				isLoading = true;
				serverThread->detach();
				serverLoadingPhotoEvent();
				return;
			}
			else  isLoading = false;

		}
		catch (...)
		{
			 
		}
	 }

	 ci::app::console()<<"==========================  SERVER ERROR SORRY.....===============================  "<<jSonstr<<std::endl;
	 isLoading = false;
	 serverThread->detach();
	 serverLoadingPhotoEvent();
}

void Server::checkConnection( )
{
	serverThread = shared_ptr<thread>( new thread( bind( &Server::checkConnectionThread, this ) ) );	
}

void Server::checkConnectionThread( )
{
	ci::ThreadSetup threadSetup;

	string status = Curl::get("http://google.com");
	
	isConnected = (status != "ERROR");	

	serverThread->detach();
	serverCheckConnectionEvent();
}

void Server::sendToMail()
{
	serverThread = shared_ptr<thread>( new thread( bind( &Server::sendToMailThread, this ) ) );	
}

void Server::sendToMailThread()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread		

	string allEmails = "yurikblech@gmail.com";

	/*for (size_t i = 0; i < emailVector.size(); i++)
	{
		if ( i != emailVector.size()-1)
		{
			allEmails +=emailVector[i] +",";
		}
		else allEmails +=emailVector[i];		
	}*/

	ci::app::console()<<"SEND TO EMAILS::  "<<allEmails<<"  sessionId  "<<sessionId<<std::endl;	

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "id" ,     sessionId));
	strings.insert(pair<string, string>( "email" ,  allEmails));

	string request =  Curl::post(serverParams::mailURL, strings);
	JsonTree jTree;

	ci::app::console()<<"SERVER ANSWER TO SEND MAILS "<<request<<std::endl;

	try 
	{
		jTree = JsonTree(request);		
		isEmailSent = jTree.getChild("success").getValue<bool>();		
	}
	catch (... )
	{	
		isEmailSent = false;
	}
	
	
	serverThread->detach();

	serverLoadingEmailEvent();
}