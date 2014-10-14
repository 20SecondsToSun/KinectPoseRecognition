#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "Curl.h"
#include "cinder/ImageIo.h"
#include "cinder/Base64.h"

namespace  serverParams 
{
	const std::string    serverURL		 =  "http://catpos.familyagency.ru/utils/upload/";
	const std::string    mailURL		 =  "http://catpos.familyagency.ru/utils/send2mail";
	
}

class Server
{
	public:
		void										sendPhoto(ci::fs::path path);
		//void										sendToServerImage();
		void										sendToServerImage( );
		std::shared_ptr<std::thread>				serverThread;

		bool										CONNECTION_PROBLEM;

		boost::signals2::signal<void(void)>			serverLoadingPhotoEvent, serverCheckConnectionEvent, serverLoadingEmailEvent;

		std::string									getBuffer(){return buffer;};
		std::string									getLink(){return link;};
		bool										isResponseOK(){return !CONNECTION_PROBLEM;};

		void										checkConnection();
		void										checkConnectionThread();


		bool										isConnected, isLoading;

		void										sendToMail();
		bool										isEmailSent;
	private:
		std::string									buffer, link;
		ci::fs::path								photoPath;

		void										sendToMailThread();


		std::string									sessionId;

};