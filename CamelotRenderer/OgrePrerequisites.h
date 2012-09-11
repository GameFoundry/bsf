/*-------------------------------------------------------------------------
This source file is a part of OGRE
(Object-oriented Graphics Rendering Engine)

For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/
#ifndef __OgrePrerequisites_H__
#define __OgrePrerequisites_H__

#include <assert.h>

// Platform-specific stuff
#include "OgrePlatform.h"

#include "OgrePortMemory.h"

// Needed for OGRE_WCHAR_T_STRINGS below
#include <string>


// configure memory tracking
#if CM_DEBUG_MODE 
#	if OGRE_MEMORY_TRACKER_DEBUG_MODE
#		define OGRE_MEMORY_TRACKER 1
#	else
#		define OGRE_MEMORY_TRACKER 0
#	endif
#else
#	if OGRE_MEMORY_TRACKER_RELEASE_MODE
#		define OGRE_MEMORY_TRACKER 1
#	else
#		define OGRE_MEMORY_TRACKER 0
#	endif
#endif




namespace Ogre {
    // Define ogre version
    #define OGRE_VERSION_MAJOR 1
    #define OGRE_VERSION_MINOR 7
    #define OGRE_VERSION_PATCH 4
	#define OGRE_VERSION_SUFFIX ""
    #define OGRE_VERSION_NAME "Cthugha"

    #define OGRE_VERSION    ((OGRE_VERSION_MAJOR << 16) | (OGRE_VERSION_MINOR << 8) | OGRE_VERSION_PATCH)

    // define the real number values to be used
    // default to use 'float' unless precompiler option set
    #if OGRE_DOUBLE_PRECISION == 1
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
        typedef double Real;
    #else
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
        typedef float Real;
    #endif

    #if CM_COMPILER == CM_COMPILER_GNUC && CM_COMP_VER >= 310 && !defined(STLPORT)
	#   if CM_COMP_VER >= 430
	#       define HashMap ::std::tr1::unordered_map
	#		define HashSet ::std::tr1::unordered_set
	#    else
	#       define HashMap ::__gnu_cxx::hash_map
	#       define HashSet ::__gnu_cxx::hash_set
	#    endif
    #else
    #   if CM_COMPILER == CM_COMPILER_MSVC
    #       if CM_COMP_VER >= 1600 // VC++ 10.0
	#			define HashMap ::std::tr1::unordered_map
	#           define HashSet ::std::tr1::unordered_set
	#		elif CM_COMP_VER > 1300 && !defined(_STLP_MSVC)
    #           define HashMap ::stdext::hash_map
	#           define HashSet ::stdext::hash_set
    #       else
    #           define HashMap ::std::hash_map
	#           define HashSet ::std::hash_set
    #       endif
    #   else
    #       define HashMap ::std::hash_map
	#       define HashSet ::std::hash_set
    #   endif
    #endif

    /** In order to avoid finger-aches :)
    */
    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef unsigned int uint;
	typedef unsigned long ulong;


	// Useful threading defines
#include "OgreThreadDefines.h"

// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
    class Angle;
    class AxisAlignedBox;
    class Camera;
    class ColourValue;
    class Degree;
    class Frustum;
    class GpuProgram;
    class GpuProgramManager;
	class GpuProgramUsage;
    class HardwareIndexBuffer;
    class HardwareOcclusionQuery;
    class HardwareVertexBuffer;
	class HardwarePixelBuffer;
	class HighLevelGpuProgram;
	class HighLevelGpuProgramManager;
	class HighLevelGpuProgramFactory;
    class IndexData;
    class Math;
    class Matrix3;
    class Matrix4;
    class Pass;
    class Plane;
    class Quaternion;
	class Radian;
    class Ray;
    class RenderSystem;
    class RenderSystemCapabilities;
    class RenderTarget;
    class RenderTexture;
	class MultiRenderTarget;
    class RenderWindow;
    class RenderOperation;
    class Sphere;
    class StringConverter;
    class StringInterface;
    class TextureState;
    class Texture;
    class TextureManager;
    class Vector2;
    class Vector3;
    class Vector4;
    class Viewport;
    class VertexBufferBinding;
    class VertexData;
    class VertexDeclaration;
}

/* Include all the standard header *after* all the configuration
settings have been made.
*/
#include "OgreStdHeaders.h"

/* Shared pointer typedefs*/
namespace Ogre
{
	typedef std::shared_ptr<GpuProgram> GpuProgramPtr;
	typedef std::shared_ptr<HighLevelGpuProgram> HighLevelGpuProgramPtr;
	typedef std::shared_ptr<HardwarePixelBuffer> HardwarePixelBufferPtr;
	typedef std::shared_ptr<Texture> TexturePtr;
}

namespace Ogre
{
	#if OGRE_WCHAR_T_STRINGS
		typedef std::wstring _StringBase;
	#else
		typedef std::string _StringBase;
	#endif

	#if OGRE_WCHAR_T_STRINGS
		typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > _StringStreamBase;
	#else
		typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
	#endif

	typedef _StringBase String;
	typedef _StringStreamBase StringStream;
	typedef StringStream stringstream;

}

//for stl containter
namespace Ogre
{ 


	template <typename T, typename A = char > 
	struct deque 
	{ 
		typedef typename std::deque<T> type;    
	}; 

	template <typename T, typename A = char > 
	struct vector 
	{ 
		typedef typename std::vector<T> type;    
	}; 

	template <typename T, typename A = char > 
	struct list 
	{ 
		typedef typename std::list<T> type;    
	}; 

	template <typename T, typename P = std::less<T>, typename A = char > 
	struct set 
	{ 
		typedef typename std::set<T, P> type;    
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = char > 
	struct map 
	{ 
		typedef typename std::map<K, V, P> type; 
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = char > 
	struct multimap 
	{ 
		typedef typename std::multimap<K, V, P> type; 
	}; 

} // Ogre

#endif // __OgrePrerequisites_H__


