#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "SocialShare.h"
#include "Utils.h"
#include "Curl.h"
#include "Server.h"

namespace facebookDefaults
{
	const std::string FACEBOOK_BASE_URL					= "https://graph.facebook.com/";

	const std::string FACEBOOK_APP_ID					= "1437302953218656";
	const std::string FACEBOOK_AUTH_URL					= "https://www.facebook.com/dialog/oauth?client_id="+ FACEBOOK_APP_ID + "&redirect_uri=http://familyagency.ru/&response_type=token&display=popup&scope=publish_actions,email,public_profile,user_friends,user_photos";
	
	const std::string FACEBOOK_FEED_URL					= FACEBOOK_BASE_URL + "me/feed";
	const std::string FACEBOOK_APP_LOGOUT_URL			= FACEBOOK_BASE_URL + "me/permissions";
	const std::string FACEBOOK_ALBUMS_URL				= FACEBOOK_BASE_URL + "me/albums";
}

namespace facebookParams
{
	const std::string ACCESS_TOKEN							= "access_token";
	const std::string NULL_ALBUM_ID							= "-1";
	const std::string ALBUM_NAME_DEFAULT				    = "Тойота Настроение";
	const std::string STATUS_DEFAULT						= "#ТойотаНастроение";
}

class Facebook: public SocShare
{
	public:
		Facebook()						{facebookAlbumNameToPost	 = facebookParams::ALBUM_NAME_DEFAULT;}

		void							post();
		const char *					getAuthUrl();
		void							logOut();

		void							setPhotoAlbumName( string &name);

	private :
			
		std::shared_ptr<std::thread>	serverThread;

		std::string						facebookAuthURL;		
		std::string						facebookAlbumNameToPost;	
		std::string						facebookAlbumId;
		std::string						facebookTextToShare;

		std::string						postPhotosToFbAlbum();		
		std::string						postPhotoFB(), postTextFB();		
		void							facebookPostThread();
		void							facebookLogOut();	

		std::string						getDefaultStatus();
};