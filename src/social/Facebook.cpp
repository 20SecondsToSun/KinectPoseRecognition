#include "Facebook.h"
using namespace mndl::curl;
using namespace facebookDefaults;
using namespace facebookParams;

void Facebook::post()
{
	serverThread = shared_ptr<thread>( new thread( bind( &Facebook::facebookPostThread, this ) ) );	
}

void Facebook::facebookPostThread()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	string status;

	switch (type)
	{
		case TEXT_STATUS:
			status = postTextFB();
		break;

		case PHOTO_STATUS:
			status =  postPhotoFB();
		break;

		default:
		break;
	}
	
	if (status == FacebookPostOk )
	{		
		logOut();
		server().sendFbSharePlus();
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

string Facebook::postTextFB()
{
	map<string,string> strings;
	strings.insert(pair<string, string>( "message" ,	Utils::cp1251_to_utf8(textStatus.c_str()) ));
	strings.insert(pair<string, string>( ACCESS_TOKEN , access_token));

	string fbRequest =  Curl::post( FACEBOOK_FEED_URL, strings);

	if (fbRequest!="")
	{
		try 
		{
			JsonTree jTree = JsonTree(fbRequest);
			if (jTree.hasChild("id"))
			{
				console()<<" POST FB OK!!!!!!!"<<std::endl;
				return FacebookPostOk;
			}
		}
		catch(...)
		{
			
		}
	}

	return FacebookStatusUpdateError;	
}

string Facebook::postPhotoFB()
{
	string fbRequest =  Curl::get(FACEBOOK_ALBUMS_URL+"/?access_token="+ access_token);	
	facebookAlbumId = NULL_ALBUM_ID;
	
	try 
	{
		JsonTree jTree = JsonTree(fbRequest);
		if (jTree.hasChild("data"))
		{
			JsonTree data = jTree.getChild("data");	
			for( JsonTree::ConstIter item = data.begin(); item != data.end(); ++item )
			{
				string _name = item->getChild( "name" ).getValue<string>();
				string _nameCyr =  Utils::Utf8_to_cp1251(_name.c_str());
				if (_nameCyr == facebookAlbumNameToPost)
				{
					facebookAlbumId = item->getChild( "id" ).getValue<string>();
				}
			}
		}
		else return FacebookAlbumAccessError;
	}
	catch(...)
	{		
		return FacebookAlbumAccessError;
	}

	if (facebookAlbumId == NULL_ALBUM_ID)
	{
		map<string,string> strings;
		strings.insert(pair<string, string>( ACCESS_TOKEN , access_token));
		strings.insert(pair<string, string>( "name" , Utils::cp1251_to_utf8(facebookAlbumNameToPost.c_str())));
		string fbRequest =  Curl::post( FACEBOOK_ALBUMS_URL, strings);

		try 
		{
			JsonTree jTree = JsonTree(fbRequest);
			if (jTree.hasChild("id"))
			{
				facebookAlbumId = jTree.getChild("id").getValue();	

				if (facebookAlbumId !="") 
				{
					return postPhotosToFbAlbum();
				}
			}
			
			return FacebookAlbumAccessError;
		}
		catch(...)
		{
			return FacebookAlbumAccessError;
		}
	}
	else
	{
		return postPhotosToFbAlbum();
	}
}

string Facebook::postPhotosToFbAlbum()
{
	int success_upload = 0;

	for (size_t i = 0, ilen = photosVector.size()  ; i < ilen; i++)
	{
		string fbRequest  =  Curl::postUploadFB(FACEBOOK_BASE_URL + facebookAlbumId+"/photos/", 
												access_token, photosVector[i],
												Utils::cp1251_to_utf8(textStatus.c_str()));	
		try 
		{
			JsonTree jTree = JsonTree(fbRequest);
			if (jTree.hasChild("post_id"))
			{
				string post_id = jTree.getChild("post_id").getValue();	
				if (post_id !="") success_upload++;			
			}
			else return FacebookPostPhotoError;
		}
		catch(...)
		{
			return FacebookPostPhotoError;
		}		
	}

	if (success_upload>0) return FacebookPostOk;

	return FacebookPostPhotoError;
}

void Facebook::setPhotoAlbumName( string &name)
{
	facebookAlbumNameToPost = name;
}

const char * Facebook::getAuthUrl()
{
	return FACEBOOK_AUTH_URL.c_str();
}

void Facebook::logOut()
{
	std::map<string,string> strings;
	strings.insert(pair<string, string>( ACCESS_TOKEN , access_token));
	string fbRequest =  Curl::deleteRequest( FACEBOOK_APP_LOGOUT_URL , strings);

	if (fbRequest != "true")
	{
		lastError = FacebookLogOutError;
	}	
}

string  Facebook::getDefaultStatus()
{
	return STATUS_DEFAULT;
}