#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "SocialShare.h"
#include "Server.h"
#include "Utils.h"
#include "Curl.h"

namespace vkontakteDefaults
{
	const std::string VKONTAKTE_APP_ID = "4610587";
	const std::string VKONTAKTE_AUTH_URL ="https://oauth.vk.com/authorize?client_id="+ VKONTAKTE_APP_ID + "&redirect_uri=https://oauth.vk.com/blank.html&scope=photos,wall&display=page&response_type=token";
	const std::string VKONTAKTE_LOGOUT_URL = "https://login.vk.com/?act=openapi&oauth=1&aid="+VKONTAKTE_APP_ID+"&location=familyagency.ru&do_logout=1&token=";
	 
    const std::string VKONTAKTE_BASE_URL					= "https://api.vk.com/";
	const std::string WALL_POST_URL							= VKONTAKTE_BASE_URL  + "method/wall.post";
	const std::string WALL_UPLOAD_SERVER_URL				= VKONTAKTE_BASE_URL  + "method/photos.getWallUploadServer";
	const std::string SAVE_WALL_PHOTO_URL					= VKONTAKTE_BASE_URL  + "method/photos.saveWallPhoto";
}

namespace vkontakteParams
{
	const std::string ACCESS_TOKEN							= "access_token";
	const std::string NULL_ALBUM_ID							= "-1";	
	const std::string STATUS_DEFAULT						= "#Котопоза";
}

class Vkontakte: public SocShare
{
	public:
		Vkontakte(){};

		void post();
		const char *getAuthUrl();
		void logOut();

	private :

		string						 vkontakteAuthURL;
	
		std::shared_ptr<std::thread> serverThread;		
		std::string					 postPhotoVK();
		std::string					 postTextVK();
		void						 vkontaktePostThread();
		std::string					 vkontaktePostLoadPhotoPath(std::string upload_url, std::string path);	

		std::string					getDefaultStatus();
};