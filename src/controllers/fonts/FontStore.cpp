#include "FontStore.h"

using namespace ci;

bool	FontStore::hasFont( const std::string &family, float size )
{	
	return (mFonts.find(std::make_pair(family,size)) != mFonts.end());
}

ci::Font*	FontStore::getFont( const std::string &family, float size )
{
	if( hasFont(family, size) )
		return mFonts[std::make_pair(family,size)];

	// return empty font on error
	return &ci::Font();
}

bool	FontStore::addFont( ci::Font* font, float size )
{
	if(!font) return false;

	// check if font family is already known
	std::string family = font->getName();
	if( ! hasFont( family, size ) ) {
		mFonts[std::make_pair(family,size)] = font;
		return true;
	}

	return false;
}

std::vector<std::pair<std::string,float>> FontStore::listFonts()
{
	std::vector<std::pair<std::string,float>> keys;

	FontList::const_iterator itr;
	for(itr=mFonts.begin();itr!=mFonts.end();++itr) 
	{
		keys.push_back( itr->first );
		//ci::app::console()<<"  keys :::::::::  "<<itr->first<<std::endl;
	}

	return keys;
}

ci::Font*	FontStore::loadFont( ci::DataSourceRef source, float size )
{
	try { 
		// try to load the file from source
		 ci::Font* font =  new Font(source, size);	

		addFont(font, size);

		return font;
	}
	catch( const std::exception &e ) {
		app::console() << "Error loading font:" << e.what() << std::endl;
	}
	
	// return empty font on error
	return  &ci::Font();
}