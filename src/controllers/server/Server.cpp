#include "Server.h"

using namespace mndl::curl;
using namespace std;
using namespace ci;

void   Server::reset()
{
	isPhotoLoaded = false;	
	isPhotoSendingToServer = false;	
	isCheckingConnection = false;	
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
	string loadingString =  toBase64(Buffer(urlRequest)) + to_string(photoParams::BIG_PHOTO_HEIGHT) +"," + to_string(Params::standID);

	ci::app::console()<<"................."<<endl;
	ci::app::console()<<loadingString<<endl;
	ci::app::console()<<"................."<<endl;
	string status;			

	#ifdef testServer	
		if(Params::serverPhotoLoadTimeout)
		{
			//status =  Curl::postImage(  serverParams::badTestURL, loadingString);
		}
		else if(Params::serverPhotoLoadError)
		{
		  // status = "ERROR";
		}
		//else
		  status =  Curl::postImage(  serverParams::serverURL, loadingString);
	#else
		status  =  Curl::postImage(  serverParams::serverURL, loadingString);
	#endif	
	
	 if (status.length()>=1)
	 {
		JsonTree jTree;
		try 
		{
			jTree = JsonTree(status);
			bool success = jTree.getChild("success").getValue<bool>();
		
			if (success==true)
			{
				ci::app::console()<<"SERVER SEND ALL FUCKING DATA  "<<status<< std::endl;
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

	 ci::app::console()<<"==========================  SERVER ERROR SORRY.....===============================  "<<status<<std::endl;
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
	if (!serverWaitingTimer.isStopped())		
		return serverParams::SERVER_WAITING_TIME - (int)serverWaitingTimer.getSeconds();
	else return serverParams::SERVER_WAITING_TIME;
}

void Server::abortLoading()
{
	serverWaitingTimer.stop();
	isPhotoLoaded = false;

	if (isPhotoSendingToServer)
	{
		isPhotoSendingToServer = false;			
		sendPhotoThread->join();
	}
	else if (isCheckingConnection)
	{
		isCheckingConnection = false;		
		checkConnectionThread->join();
	}
	else if (isMailSending)
	{
		isMailSending = false;
		//sendToMailThread->join();
	}
}

void Server::checkConnection( )
{	
	serverWaitingTimer.start();	
	checkConnectionThread = std::shared_ptr<boost::thread>( new boost::thread( bind( &Server::checkConnectionThreadHandler, this ) ) );	
}

void Server::checkConnectionThreadHandler( )
{
	/*isCheckingConnection = true;
	string status;
	#ifdef testServer	
		if(Params::serverConnectionCheckTimeout)
			status = Curl::get(serverParams::badTestURL);	
		else if(Params::serverConnectionCheckError)			
		   status = "ERROR";	
		else
		  status = Curl::get(serverParams::connectionTestURL);
	#else*/
	string status = Curl::get(serverParams::connectionTestURL);	
	//#endif

	isConnected = (status != "ERROR");		
	serverCheckConnectionEvent();
	isCheckingConnection = false;
}

void Server::sendToMail(string mails)
{
	serverWaitingTimer.start();	
	sendToMailThread = std::shared_ptr<boost::thread>( new boost::thread( bind( &Server::sendToMailThreadHandler, this, mails ) ) );	
}

void Server::sendToMailThreadHandler(string allEmails)
{	
	ci::app::console()<<"SEND TO EMAILS::  "<<allEmails<<"  sessionId  "<<sessionId<<std::endl;	

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "id" ,     sessionId));
	strings.insert(pair<string, string>( "email" ,  allEmails));

	//;
	string status;

	#ifdef testServer	
		if(Params::serverEmailSendTimeout)
		{
			//status =  Curl::post(serverParams::badTestURL, strings);
		}
		else if(Params::serverEmailSendError)
		{
		  // status = "ERROR";
		}
		//else
		  status =  Curl::post(serverParams::mailURL, strings);
	#else
		 status =  Curl::post(serverParams::mailURL, strings);
	#endif	

	JsonTree jTree;

	ci::app::console()<<"SERVER ANSWER TO SEND MAILS "<<status<<std::endl;

	try 
	{
		jTree = JsonTree(status);		
		isEmailSent = jTree.getChild("success").getValue<bool>();		
	}
	catch (... )
	{	
		isEmailSent = false;
	}

	sendToMailEvent();
}

std::string   Server::sendVkSharePlus()
{	
	std::map<string,string> strings;
	//strings.insert(pair<string, string>( "action" , "cnt"));
	strings.insert(pair<string, string>( "id" ,   to_string(Params::standID)));
	strings.insert(pair<string, string>( "type" ,  "vk"));
	string request =  Curl::post( serverParams::shareSaveURL, strings);

	JsonTree jTree;

	try 
	{
		console()<<"Try to Vkontakte PLUS   "<<request<<endl;
		jTree = JsonTree(request);
		console()<<"VKONTAKTE PLUS   "<< jTree.getChild("success").getValue()<<std::endl;
		return SERVER_OK;
	}
	catch(...)
	{

	}

	return SERVER_ERROR;
}

std::string   Server::sendFbSharePlus()
{	
	std::map<string,string> strings;
	//strings.insert(pair<string, string>( "action" , "cnt"));
	strings.insert(pair<string, string>( "id" ,   to_string(Params::standID)));
	strings.insert(pair<string, string>( "type" ,  "fb"));
	string request =  Curl::post( serverParams::shareSaveURL, strings);

	JsonTree jTree;

	try 
	{
		console()<<"Try to Facebook PLUS   "<<request<<endl;
		jTree = JsonTree(request);
		console()<<"Facebook PLUS   "<< jTree.getChild("success").getValue()<<std::endl;
		return SERVER_OK;
	}
	catch(...)
	{

	}
	return SERVER_ERROR;
}

std::string Server::sendToServerPrintInfo()
{
	std::map<string,string> strings;
	strings.insert(pair<string, string>( "action" , "print_count"));
	strings.insert(pair<string, string>( "cnt" ,  "1"));	
	string request =  Curl::post( serverParams::printerCountPlusURL, strings);

	JsonTree jTree;

	try 
	{
		jTree = JsonTree(request);
		console()<<"PHOTO NUMS   "<< jTree.getChild("cnt").getValue()<<std::endl;
		return SERVER_OK;
	}
	catch(...)
	{

	}

	return SERVER_ERROR;
}