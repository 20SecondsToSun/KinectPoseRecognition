/*
 Copyright (C)2011 Paul Houx
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, 
 are permitted without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/DataSource.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"

#include <boost/thread.hpp>
#include <boost/functional/hash.hpp>

namespace ph
{
	// add a getUseCount() function to the gl::Texture class by extending it
	class Texture : public ci::gl::Texture
	{
	public:
		//! Default initializer. Points to a null Obj
		Texture() : ci::gl::Texture(){};		
		/** \brief Constructs a texture of size(\a aWidth, \a aHeight), storing the data in internal format \a aInternalFormat. **/
		Texture( int aWidth, int aHeight, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(aWidth, aHeight, format){};
		/** \brief Constructs a texture of size(\a aWidth, \a aHeight), storing the data in internal format \a aInternalFormat. Pixel data is provided by \a data and is expected to be interleaved and in format \a dataFormat, for which \c GL_RGB or \c GL_RGBA would be typical values. **/
		Texture( const unsigned char *data, int dataFormat, int aWidth, int aHeight, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(data, dataFormat, aWidth, aHeight, format){};
		/** \brief Constructs a texture based on the contents of \a surface. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
		Texture( const ci::Surface8u &surface, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(surface, format){};
		/** \brief Constructs a texture based on the contents of \a surface. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
		Texture( const ci::Surface32f &surface, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(surface, format){};
		/** \brief Constructs a texture based on the contents of \a channel. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
		Texture( const ci::Channel8u &channel, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(channel, format){};
		/** \brief Constructs a texture based on the contents of \a channel. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
		Texture( const ci::Channel32f &channel, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(channel, format){};
		/** \brief Constructs a texture based on \a imageSource. A default value of -1 for \a internalFormat chooses an appropriate internal format based on the contents of \a imageSource. **/
		Texture( ci::ImageSourceRef imageSource, ci::gl::Texture::Format format = Format() ) : ci::gl::Texture(imageSource, format){};
		//! Constructs a Texture based on an externally initialized OpenGL texture. \a aDoNotDispose specifies whether the Texture is responsible for disposing of the associated OpenGL resource.
		Texture( GLenum aTarget, GLuint aTextureID, int aWidth, int aHeight, bool aDoNotDispose ) : ci::gl::Texture(aTarget, aTextureID, aWidth, aHeight, aDoNotDispose){};
		
		//! provide useCount for memory management
		long getUseCount(){ return mObj.use_count(); };
	};

	class TextureManager
	{
	private:
		TextureManager(void);
		virtual ~TextureManager(void);
	public:	
		// singleton implementation
		static TextureManager& getInstance() { static TextureManager tm; return tm; };

		//! synchronously loads an image into a texture, stores it and returns it
		ci::gl::Texture	load(const std::string &url, ci::gl::Texture::Format fmt=ci::gl::Texture::Format());
		//! stores an already loaded texture, so other objects can use it as well
		void			store(const std::string &url, ci::gl::Texture texture);
		//! asynchronously loads an image into a texture, returns immediately
		ci::gl::Texture	fetch(const std::string &url, ci::gl::Texture::Format fmt=ci::gl::Texture::Format()); 
		//! remove url from the queue. Has no effect if image has already been loaded
		bool			abort(const std::string &url);

		void			clear();

		//! returns a list of valid extensions for image files. ImageIO::getLoadExtensions() does not seem to work.
		std::vector<std::string>	getLoadExtensions();

		//! returns TRUE if image is scheduled for loading but has not been turned into a Texture yet
		bool isLoading(const std::string &url);
		//! returns TRUE if image has been turned into a Texture
		bool isLoaded(const std::string &url);

		//! returns an empty texture. Override it to supply something else in case a texture was not available.
		virtual ci::gl::Texture	empty(){ return ci::gl::Texture(); };
	protected:
		//!
		boost::hash<std::string> hash;

		//! list of created Textures
		std::map<std::size_t, ph::Texture>	mTextures;

		//! queue of textures to load asynchronously
		std::deque<std::string>				mQueue;
		boost::mutex						mQueueMutex;

		//!	container for the asynchronously loaded surfaces	
		std::map<std::string, ci::Surface>	mSurfaces;
		boost::mutex						mSurfaceMutex;
		//!
		boost::shared_ptr<boost::thread>	mThread;

	private:
		//! loads files in a separate thread and creates Surfaces. These are then passed to the main thread and turned into Textures.
		void threadLoad();
		//! removes Textures from memory if no longer in use
		void garbageCollect();
	};

	// helper functions for easier access 
	
	//! synchronously loads an image into a texture, stores it and returns it 
	inline ci::gl::Texture	loadTexture(const std::string &url, ci::gl::Texture::Format fmt=ci::gl::Texture::Format()){ return TextureManager::getInstance().load(url, fmt); };
	
	//! stores an already loaded texture, so other objects can use it as well 
	inline void				storeTexture(const std::string &url, ci::gl::Texture texture){ return TextureManager::getInstance().store(url, texture); };
	
	//! asynchronously loads an image into a texture, returns immediately 
	inline ci::gl::Texture	fetchTexture(const std::string &url, ci::gl::Texture::Format fmt=ci::gl::Texture::Format()){ return TextureManager::getInstance().fetch(url, fmt); };


	inline void 	clearTexture(){ return TextureManager::getInstance().clear(); };
}
