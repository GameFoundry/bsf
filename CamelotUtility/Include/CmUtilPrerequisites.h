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
#include "CmTypes.h"

namespace Ogre {
    #if OGRE_COMPILER == OGRE_COMPILER_GNUC && OGRE_COMP_VER >= 310 && !defined(STLPORT)
	#   if OGRE_COMP_VER >= 430
	#       define HashMap ::std::tr1::unordered_map
	#		define HashSet ::std::tr1::unordered_set
	#    else
	#       define HashMap ::__gnu_cxx::hash_map
	#       define HashSet ::__gnu_cxx::hash_set
	#    endif
    #else
    #   if OGRE_COMPILER == OGRE_COMPILER_MSVC
    #       if OGRE_COMP_VER >= 1600 // VC++ 10.0
	#			define HashMap ::std::tr1::unordered_map
	#           define HashSet ::std::tr1::unordered_set
	#		elif OGRE_COMP_VER > 1300 && !defined(_STLP_MSVC)
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

	// Useful threading defines
#include "OgreThreadDefines.h"

// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
    class Angle;
    class AxisAlignedBox;
    class Degree;
    class Math;
    class Matrix3;
    class Matrix4;
    class Plane;
    class Quaternion;
	class Radian;
    class Ray;
    class Sphere;
    class Vector2;
    class Vector3;
    class Vector4;
}

/* Include all the standard header *after* all the configuration
settings have been made.
*/
#include "OgreStdHeaders.h"

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


