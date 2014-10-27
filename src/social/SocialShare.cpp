#include "SocialShare.h"

void SocShare::setLoginPassword(std::string _login, std::string _password)
{
	login = _login;
	password = _password;
}

void SocShare::postStatus(std::string _textStatus)
{
	type = TEXT_STATUS;
	textStatus = _textStatus ==""?getDefaultStatus():_textStatus;

	post();	
}

void SocShare::postPhoto(std::string path, std::string _textStatus)
{
	type = PHOTO_STATUS;
	textStatus = _textStatus ==""?getDefaultStatus():_textStatus;
	photosVector.clear();
	photosVector.push_back(path);

	post();
}

void SocShare::postPhoto(std::vector<std::string> path, std::string _textStatus)
{
	type = PHOTO_STATUS;
	textStatus = _textStatus;
	photosVector.clear();
	photosVector = path;

	post();
}

void SocShare::clear_token()
{
	access_token = "";
}

std::string SocShare::getResponse()
{
	return response;
}

std::string SocShare::getLastError()
{
	return lastError;
}