#pragma once
#include "cinder/app/AppNative.h"

#define SOCIAL_NONE      0
#define SOCIAL_VKONTAKTE 1
#define SOCIAL_FACEBOOK  2
#define SOCIAL_TWITTER	 3
#define SOCIAL_EMAIL	 4

#define	WallUploadVkontakteServerError			"WallUploadVkontakteServerError"
#define	WallPostVkontakteServerError			"WallPostVkontakteServerError"
#define	UploadVkontakteServerError				"UploadVkontakteServerError"
#define	SaveWallPhotoVkontakteServerError		"SaveWallPhotoVkontakteServerError"

#define	FacebookAlbumAccessError				"FacebookAlbumAccessError"
#define	FacebookError							"FacebookError"
#define FacebookPostPhotoError					"FacebookPostPhotoError"
#define FacebookLogOutError						"FacebookLogOutError"
#define FacebookStatusUpdateError				"FacebookStatusUpdateError"


#define	VkontaktePostOk							"OK"
#define	FacebookPostOk							"OK"
#define	SocialPostOk							"OK"
#define	SocialPostError							"Error"

#define	TEXT_STATUS  1
#define	PHOTO_STATUS 2


class SocShare
{
	public:

		virtual void								post() = 0;		
		virtual const char *						getAuthUrl() = 0;
		virtual void								logOut() = 0;

		void										clear_token();
		
		void										postStatus(std::string textStatus = "") ;
		void										postPhoto(std::string path, std::string textStatus = "");		
		void										postPhoto(std::vector<std::string> path, std::string textStatus);
		void										setLoginPassword(std::string _login, std::string _password);	

		std::string									access_token;			
		std::string									getResponse();
		std::string									response;

		boost::signals2::signal<void(void)>			serverHandler;		

		std::string									getLastError();

	protected:

		int											type;
		std::vector<std::string>					photosVector;
		std::string									login, password, textStatus;
		virtual std::string							getDefaultStatus() = 0;	
		std::string									lastError;
};