#ifndef __CURL_H_INCLUDED__
#define __CURL_H_INCLUDED__

#include <map>
#include <string>
#include <vector>
#include "cinder/app/AppNative.h"
#include "cinder/Filesystem.h"

#include <curl/curl.h>



namespace mndl { namespace curl {

class Curl
{
public:
	Curl(){};
	~Curl(){};
	static std::string post( const std::string &url, std::vector<std::string>           &params );
	static std::string post( const std::string &url, std::map<std::string, std::string> &params );
	static std::string	deleteRequest(const std::string &url,std:: map<std::string,std::string> &m);
	static std::string postUpload( const  std::string &url, std::map<std::string, std::string> &params, std::string path );
	static std::string postUploadFB( const std::string &url, const std::string &  ACCESSTOKEN, const std::string &  path, const std::string &  message);
	
	static std::string postImage( const std::string &Serverurl, const std::string &Imageurl );
	static std::string sendMail( );

	static std::string get ( const std::string &url );
	static bool       ftpUpload( const std::string &ftpUrl, const std::string &userName, const std::string &password, const ci::fs::path &path );
	
	static CURL * curl; 
	static void clean ();

protected:
	static int writer( char *data, size_t size, size_t nmemb, std::string *buffer );
	static std::string easyCurl( const std::string &url, bool post, const std::string &postParamString );
	static std::string easyCurlUpload( const std::string &url, bool post, const std::string &postParamString, std::FILE* file ); 	
	static std::string urlEncode( const std::string &c );
	static std::string char2Hex( char dec );
};

} }// namespace mndl::curl

#endif // __CURL_H_INCLUDED__