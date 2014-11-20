#include "Vkontakte.h"
using namespace mndl::curl;
using namespace vkontakteDefaults;
using namespace vkontakteParams;

void   Vkontakte::post()
{
	serverThread = shared_ptr<thread>( new thread( bind( &Vkontakte::vkontaktePostThread, this ) ) );	
}

void   Vkontakte::vkontaktePostThread()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	string status;

	switch (type)
	{
	case TEXT_STATUS:
		status = postTextVK();			
		break;

	case PHOTO_STATUS:
		status =  postPhotoVK();			
		break;

	default:
		break;
	}

	if (status == VkontaktePostOk )
	{
		logOut();
		server().sendVkSharePlus();		
		response = SocialPostOk;
	}
	else
	{
		logOut();
		response = SocialPostError;	
		lastError = status;
	}

	serverThread->detach();	

	serverHandler();
}

string Vkontakte::postTextVK()
{
	std::map<string,string> strings;
	strings.insert(pair<string, string>( "message" ,	  Utils::cp1251_to_utf8(textStatus.c_str()) ));
	strings.insert(pair<string, string>( ACCESS_TOKEN , access_token));

	string vkRequest =  Curl::post( WALL_POST_URL, strings);

	if (vkRequest!="")
	{
		try 
		{
			JsonTree jTree = JsonTree(vkRequest);
			if (jTree.hasChild("response"))
			{
				if ( jTree.getChild("response").hasChild("post_id"))
					return VkontaktePostOk;
			}
		}
		catch(...)
		{

		}
	}

	return WallPostVkontakteServerError;	
}

string Vkontakte::postPhotoVK()
{
	std::map<string,string> strings;
	strings.insert(pair<string, string>( ACCESS_TOKEN , access_token));

	string vkRequest =  Curl::post( WALL_UPLOAD_SERVER_URL, strings);		
	string upload_url = "";
	try 
	{
		JsonTree jTree = JsonTree(vkRequest);
		if (jTree.hasChild("response"))
		{
			upload_url = jTree.getChild("response").getChild("upload_url").getValue();
		}
		else WallUploadVkontakteServerError;
	}
	catch(...)
	{
		return WallUploadVkontakteServerError;
	}

	string attacments = "";

	for (size_t i = 0, ilen = photosVector.size()  ; i < ilen; i++)
	{	
		string photo_id = vkontaktePostLoadPhotoPath(upload_url, photosVector[i]);
		if (photo_id != "ERROR") 
		{
			attacments += photo_id;
			if (i != 3) attacments += ",";
		}
	}

	console()<<"ATTACHMENTS:::  "<< attacments<<std::endl;

	strings.clear();	
	strings.insert(pair<string, string>( "attachments" ,  attacments));		
	strings.insert(pair<string, string>( "message" ,	  Utils::cp1251_to_utf8(textStatus.c_str()) ));
	strings.insert(pair<string, string>( "access_token" , access_token));

	vkRequest =  Curl::post(WALL_POST_URL, strings);

	if (vkRequest!="")
	{
		try 
		{
			JsonTree jTree = JsonTree(vkRequest);
			if (jTree.hasChild("response"))
			{
				if ( jTree.getChild("response").hasChild("post_id"))
					return VkontaktePostOk;
			}
		}
		catch(...)
		{

		}
	}

	return WallPostVkontakteServerError;	
}

string Vkontakte::vkontaktePostLoadPhotoPath(string upload_url, string path)
{
	std::map<string,string> strings;
	strings.insert(pair<string, string>( ACCESS_TOKEN , access_token));

	string vkRequest  =  Curl::postUpload( upload_url, strings, path);

	JsonTree jTree;
	try
	{
		jTree  = JsonTree(vkRequest);

		strings.clear();
		strings.insert(pair<string, string>( "server" , jTree.getChild("server").getValue() ));
		strings.insert(pair<string, string>( "photo" , jTree.getChild("photo").getValue() ));
		strings.insert(pair<string, string>( "hash" , jTree.getChild("hash").getValue() ));
		strings.insert(pair<string, string>( ACCESS_TOKEN , access_token ));
	}
	catch(...)
	{
		return  UploadVkontakteServerError;	
	}

	vkRequest =  Curl::post( SAVE_WALL_PHOTO_URL, strings);
	string photo_id="";

	try
	{
		jTree = JsonTree(vkRequest);

		if (jTree.hasChild( "response[0]" ) )	
		{
			photo_id = jTree.getChild( "response[0]" ).getChild( "id" ).getValue();
		}
		else 	return SaveWallPhotoVkontakteServerError;
	}
	catch(...)
	{
		return SaveWallPhotoVkontakteServerError;
	}

	return photo_id;
}

const char * Vkontakte::getAuthUrl()
{	
	return VKONTAKTE_AUTH_URL.c_str();
}

void Vkontakte::logOut()
{
	string  logout = VKONTAKTE_LOGOUT_URL + access_token;
	const char * url = logout.c_str();
	string vkRequest =  Curl::get(url);	
}

string Vkontakte::getDefaultStatus()
{
	return STATUS_DEFAULT;
}