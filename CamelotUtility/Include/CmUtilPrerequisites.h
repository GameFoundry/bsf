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
#pragma once

#include <assert.h>

// Platform-specific stuff
#include "CmPlatform.h"

// Short-hand names for various built-in types
#include "CmTypes.h"

// Useful threading defines
#include "CmThreadDefines.h"

// Forward declarations
#include "CmUtilFwdDecl.h"

// Commonly used standard headers
#include "CmStdHeaders.h"

// Standard containers, for easier access in my own namespace
namespace CamelotEngine
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

}
