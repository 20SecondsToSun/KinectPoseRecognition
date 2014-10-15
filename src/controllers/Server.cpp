#include "Server.h"

using namespace mndl::curl;
using namespace std;
using namespace ci;

void   Server::reset()
{
	isPhotoLoaded = false;	
	isPhotoSendingToServer = false;	
}

void   Server::sendPhoto(fs::path _path)
{
	serverWaitingTimer.start();	
	photoPath =_path;
	sendPhotoThread = std::shared_ptr< boost::thread>( new  boost::thread( bind( &Server::sendPhotoThreadHandler, this ) ) );
}

void Server::sendPhotoThreadHandler()
{	
	isPhotoSendingToServer = true;	
	DataSourceRef urlRequest =	loadFile( photoPath);
	string loadingString =  toBase64(Buffer(urlRequest)) + to_string(638) +",1";

	ci::app::console()<<".............  "<<loadingString<<endl;

	string jSonstr			 =  Curl::postImage(  serverParams::serverURL, loadingString);
	//	try { boost::this_thread::interruption_point(); }
	//			catch(boost::thread_interrupted) { break; }
	
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
				
				isPhotoLoaded = true;
				isPhotoSendingToServer = false;
				serverLoadingPhotoEvent();
				return;
			}
			else  isPhotoLoaded = false;

		}
		catch (...)
		{
			 
		}
	 }

	 ci::app::console()<<"==========================  SERVER ERROR SORRY.....===============================  "<<jSonstr<<std::endl;
	 isPhotoLoaded = false;	
	 isPhotoSendingToServer = false;
	 serverLoadingPhotoEvent();	
}

void Server::stopTimeout( )
{
	serverWaitingTimer.stop();
}

bool Server::timerIsRunning( )
{
	return !serverWaitingTimer.isStopped();
}

int Server::getTimeoutSeconds( )
{
	return serverParams::SERVER_WAITING_TIME - (int)serverWaitingTimer.getSeconds();
}

void Server::abortLoading()
{
	serverWaitingTimer.stop();
	isPhotoLoaded = false;

	if (isPhotoSendingToServer)
	{
		isPhotoSendingToServer = false;
		sendPhotoThread->interrupt();
		sendPhotoThread->join();
	}

	if (isCheckingConnection)
	{
		isCheckingConnection = false;
		checkConnectionThread->interrupt();
		checkConnectionThread->join();
	}

	if (isMailSending)
	{
		isMailSending = false;
	}
}

void Server::checkConnection( )
{	
	checkConnectionThread = std::shared_ptr<boost::thread>( new boost::thread( bind( &Server::checkConnectionThreadHandler, this ) ) );	
}

void Server::checkConnectionThreadHandler( )
{
	isCheckingConnection = true;
	string status = Curl::get("http://google.com");	
	isConnected = (status != "ERROR");		
	serverCheckConnectionEvent();
	isCheckingConnection = false;
}

void Server::sendToMail()
{
	sendToMailThread = std::shared_ptr<boost::thread>( new boost::thread( bind( &Server::sendToMailThreadHandler, this ) ) );	
}

void Server::sendToMailThreadHandler()
{	
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

	sendToMailEvent();
}