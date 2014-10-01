#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include <map>


typedef std::map<std::pair<std::string,float>, ci::Font*>	FontList;

class FontStore
{
private:
	FontStore() {};
	~FontStore() {};
public:
	// singleton implementation
	static FontStore& getInstance() { 
		static FontStore fm; 
		return fm; 
	};

	bool		hasFont( const std::string &family, float size );
	ci::Font*	getFont( const std::string &family, float size );

	//!
	bool		addFont( ci::Font* font, float size );

	//! returns a vector with all available font families
	std::vector<std::pair<std::string,float>>	listFonts();

	ci::Font*	loadFont( ci::DataSourceRef source, float size );

protected:
	FontList	mFonts;
};

// helper function(s) for easier access 
inline FontStore&	fonts() { return FontStore::getInstance(); };