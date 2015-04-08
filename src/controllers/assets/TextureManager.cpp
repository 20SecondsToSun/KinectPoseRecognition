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

// get rid of a very annoying warning caused by boost::thread::sleep()
#pragma warning(push)
#pragma warning(disable: 4244)

#include "TextureManager.h"
#include "cinder/ip/Resize.h"

using namespace ph;

using namespace ci;
using namespace ci::app;
using namespace std;

TextureManager::TextureManager(void)
{
	// initialize buffers
	mQueue.clear();
	mTextures.clear();
	mSurfaces.clear();

	// start loader thread
	mThread = boost::shared_ptr<boost::thread>(new boost::thread(&TextureManager::threadLoad, this));
}

TextureManager::~TextureManager(void)
{
	// stop loader thread and wait for it to finish
	mThread->interrupt();
	mThread->join();

	// clear buffers
	mQueue.clear();
	mTextures.clear();
	mSurfaces.clear();
}

void TextureManager::clear()
{
	// clear buffers
	mQueue.clear();
	mTextures.clear();
	mSurfaces.clear();
}

gl::Texture TextureManager::load(const string &url, gl::Texture::Format fmt)
{
	// check if texture is in TextureList
	if(isLoaded(url)) return mTextures[ hash(url) ];

	// remove from loading queue
	abort(url);

	// perform garbage collection to make room for new textures
	garbageCollect();

	// load texture and add to TextureList
	console() << "Loading Texture '" << url << "'." << endl;
	try
	{
		ImageSourceRef img = loadImage(url);
		ph::Texture texture = ph::Texture( img, fmt );
		mTextures[ hash(url) ] = texture;

		return texture;
	}
	catch(...){}

	try
	{
		ImageSourceRef img = loadImage( loadUrl( Url(url) ) );
		ph::Texture texture = ph::Texture( img, fmt );
		mTextures[ hash(url) ] = texture;

		return texture;
	}
	catch(...){}

	// did not succeed
	console() << "Error loading texture '" << url << "'!" << endl;
	return gl::Texture();
}

void TextureManager::store(const string &url, gl::Texture texture)
{
	if(!isLoaded(url))
	{
		// perform garbage collection to make room for new textures
		garbageCollect();

		// remove from loading queue
		abort(url);

		console() << "Storing texture '" << url << "'." << endl;
		mTextures[ hash(url) ] = ph::Texture( texture );
	}
}

gl::Texture TextureManager::fetch(const string &url, gl::Texture::Format fmt)
{
	std::size_t hashedUrl = hash(url);

	// check if surface has loaded and convert it into texture
	mSurfaceMutex.lock();
	if(mSurfaces.find(url) != mSurfaces.end()) {
		console() << "Creating Texture for '" << url << "'." << endl;

		// create texture from most recently loaded surface only
		ph::Texture tex = ph::Texture(mSurfaces[url], fmt);
		if(tex) mTextures[ hashedUrl ] = tex;

		// remove from map
		mSurfaces.erase(url);
	}
	mSurfaceMutex.unlock();

	// check if texture is already loaded
	if(isLoaded(url)) return mTextures[ hashedUrl ];

	// perform garbage collection to make room for new textures
	garbageCollect();

	// add to loading queue		
	mQueueMutex.lock();
	if( std::find(mQueue.begin(), mQueue.end(), url) == mQueue.end() ) {
		console() << "Queueing Texture '" << url << "' for loading." << endl;
		mQueue.push_back(url);	
	}
	mQueueMutex.unlock();

	return empty();
}

bool TextureManager::abort(const string &url)
{
	bool aborted = false;

	// remove from loading queue
	mQueueMutex.lock();
	std::deque<std::string>::iterator itr = std::find(mQueue.begin(), mQueue.end(), url);
	if(itr != mQueue.begin() && itr != mQueue.end()) {
		mQueue.erase(itr);
		aborted = true;
	}
	mQueueMutex.unlock();

	return aborted;
}

vector<string> TextureManager::getLoadExtensions()
{
	// TODO: ImageIO::getLoadExtensions() doesn't work, but use that instead once it does

	vector<string> result;
	result.push_back(".jpg");
	result.push_back(".png");

	return result;
}

bool TextureManager::isLoading(const string &url)
{
	// check if texture exists
	if( mTextures.find( hash(url) ) != mTextures.end() ) 
		return false;

	// check if just loaded but not a texture yet
	mSurfaceMutex.lock();
	bool loaded = ( mSurfaces.find(url) != mSurfaces.end() );
	mSurfaceMutex.unlock();
	if(loaded) return false;

	// check if queued for loading
	mQueueMutex.lock();
	bool queued = (std::find(mQueue.begin(), mQueue.end(), url) != mQueue.end());
	mQueueMutex.unlock();

	return queued;
}

bool TextureManager::isLoaded(const string &url)
{
	if(mTextures.empty()) return false;

	// find texture
	return (mTextures.find( hash(url) ) != mTextures.end());
}

void TextureManager::garbageCollect()
{
	//console()<<"GARBAGE COLLECT::::::::::::::::::   "<<mTextures.size()<<endl;
	for(std::map<std::size_t, ph::Texture>::iterator itr=mTextures.begin();itr!=mTextures.end();)
	{
		// Cinder hack added to 'gl::Texture':
		//		public: long getUseCount(){ return mObj.use_count(); };
		if(itr->second.getUseCount() < 2)
		{
			console() << "Removing texture '" << itr->first << "' because it is no longer in use." << endl;
			itr = mTextures.erase(itr);
		}
		else
			++itr;
	}
}

//

void TextureManager::threadLoad()
{
	bool			empty;
	bool			succeeded;
	Surface			surface;
	ImageSourceRef	image;
	string			url;

	// run until interrupted
	while(true) {
		// fetch first url from the queue, if any	
		mQueueMutex.lock();
		empty = mQueue.empty();
		if(!empty) url = mQueue.front();
		mQueueMutex.unlock();

		if(!empty) {
			// try to load image
			succeeded = false;

			// try to load from FILE
			if(!succeeded) try { 
				image = ci::loadImage( ci::loadFile( url ) ); 
				succeeded = true;
			} catch(...) {}

			// try to load from URL
			if(!succeeded) try { 
				image = ci::loadImage( ci::loadUrl( Url(url) ) ); 
				succeeded = true;
			} catch(...) {}

			if(!succeeded) {
				// both attempts to load the url failed

				// remove url from queue
				mQueueMutex.lock();
				mQueue.pop_front();
				mQueueMutex.unlock();

				continue;
			}

			// succeeded, check if thread was interrupted
			try { boost::this_thread::interruption_point(); }
			catch(boost::thread_interrupted) { break; }

			// create Surface from the image
			surface = Surface(image);

			// check if thread was interrupted
			try { boost::this_thread::interruption_point(); }
			catch(boost::thread_interrupted) { break; }

			// resize image if larger than 4096 px
			Area source = surface.getBounds();
			Area dest(0, 0, 4096, 4096);
			Area fit = Area::proportionalFit(source, dest, false, false);
				
			if(source.getSize() != fit.getSize()) 
				surface = ci::ip::resizeCopy(surface, source, fit.getSize());

			// check if thread was interrupted
			try { boost::this_thread::interruption_point(); }
			catch(boost::thread_interrupted) { break; }

			// copy to main thread
			mSurfaceMutex.lock();
			mSurfaces[url] = surface;
			mSurfaceMutex.unlock();	

			// remove url from queue
			mQueueMutex.lock();
			mQueue.pop_front();
			mQueueMutex.unlock();
		}

		// sleep a while
		boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
	}
}

//
#pragma warning(pop)
