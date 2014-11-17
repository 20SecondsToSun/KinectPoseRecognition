#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "Curl.h"
#include "cinder/ImageIo.h"
#include "cinder/Base64.h"
#include <boost/thread.hpp>
#include "Params.h"

//#define SERVER "92.63.102.99:82"
#define SERVER_OK "OK"
#define SERVER_ERROR "ERROR"

namespace serverParams
{
	static const int	 SERVER_WAITING_TIME = 10;
	const std::string    connectionTestURL	 =  "http://google.com";
	const std::string    serverURL			 =  "http://kotopoza.ru/utils/upload/";
	const std::string    shareSaveURL		 =  "http://kotopoza.ru/utils/socnt/";
	const std::string    printerCountPlusURL =  "http://kotopoza.ru/utils/save/";	
	const std::string    mailURL			 =  "http://kotopoza.ru/utils/send2mail";
	const std::string    badTestURL			 =  "http://catpos.familyagency.ru/utils/upload1/";
}

class Server
{
	public:	
		static Server& getInstance() { static Server _server; return _server; };

		void										checkConnection();
		void										sendPhoto(ci::fs::path path);
		void										sendToMail(std::string mails);

		void										reset();
		std::string									getBuffer(){return buffer;};
		std::string									getLink(){return link;};			

		bool										isConnected, isPhotoLoaded;
		bool										isEmailSent;

		void										stopTimeout();
		int											getTimeoutSeconds();
		bool										timerIsRunning();
		void										abortLoading();

		boost::signals2::signal<void(void)>			serverLoadingPhotoEvent;
		boost::signals2::signal<void(void)>			serverCheckConnectionEvent;
		boost::signals2::signal<void(void)>			sendToMailEvent;

		std::string  sendVkSharePlus();
		std::string  sendFbSharePlus();
		std::string  sendToServerPrintInfo();

	private:

		std::shared_ptr<boost::thread>				sendPhotoThread;
		void										sendPhotoThreadHandler( );		
	
		std::shared_ptr<boost::thread>				checkConnectionThread;
		void										checkConnectionThreadHandler();
		

		std::shared_ptr<boost::thread>				sendToMailThread;
		void										sendToMailThreadHandler(std::string emailVector);
		

		std::string									buffer, link;
		ci::fs::path								photoPath;
		std::string									sessionId;

		ci::Timer									serverWaitingTimer;
		bool										isPhotoSendingToServer, isCheckingConnection, isMailSending;

		
};

inline Server&	server() { return Server::getInstance(); };