#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/CinderMath.h"
#include "cinder/ImageIo.h"
#include "cinder/Base64.h"
#include "cinder/Text.h"
#include "cinder/ip/Resize.h"
#include "cinder/gl/Fbo.h"
#include "cinder/params/Params.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Json.h"
#include "cinder/ip/Resize.h"
#include "cinder/Exception.h"
#include "cinder/Filesystem.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Timeline.h"

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/thread.hpp>

#include <stdint.h>