#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"
#include "cinder/ImageIo.h"
#include "cinder/Base64.h"
#include "cinder/Text.h"
#include "cinder/ip/Resize.h"
#include "cinder/gl/Fbo.h"

using namespace std;
using namespace ci;
using namespace ci::gl;
using namespace ci::app;

class Utils
{
	public:

		static struct tm		getCurrentTime();	
		static std::string  Utils::duplicateSymbol(std::string str, std::string symb);
		static gl::Texture		loadImageFromString(std::string value);

		static int  spc_email_isvalid(const char *address);
		static bool isValidEmail(std::string _email);
		static bool isValidTel(std::string tel);

		static string cp1251_to_utf8(const char *str);
		static string Utf8_to_cp1251(const char *str);

		static std::wstring s2ws(const std::string& s);
	    static LPWSTR	ConvertToLPWSTR( const std::wstring& s );
		static int		StringToWString(std::wstring &ws, const std::string &s);
		static std::string twiceSymbol(std::string __string, char __symbol);


		static bool readFlag(std::string path);
		static void writeCrashLog(std::string path);

		static std::string		urlDecode(std::string SRC);

		static void textFieldDraw(std::string text, ci::Font* font, ci::Vec2f coords, ci::ColorA color);

		static float map(float value, 
                              float istart, 
                              float istop, 
                              float ostart, 
                              float ostop);

		static float clamp(float value, float max, float min);

		static ci::Surface Utils::resizeScreenshot(ci::Surface screenshot, int32_t width, int32_t height);


		static gl::Texture  getTextField(std::string text,ci::Font* font,  ci::ColorA color);
		std::string  getCorrectSecondsText(int seconds);

		static void  drawGraphicsToFBO(Fbo fbo, const function<void ()>& graphicsFunc);

		static void clearFBO(Fbo fbo);
		static void printVideoMemoryInfo();
};