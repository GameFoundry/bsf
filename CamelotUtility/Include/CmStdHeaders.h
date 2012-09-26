#pragma once

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

#if defined ( CM_GCC_VISIBILITY ) && (CM_PLATFORM != CM_PLATFORM_APPLE)
/* Until libstdc++ for gcc 4.2 is released, we have to declare all
 * symbols in libstdc++.so externally visible, otherwise we end up
 * with them marked as hidden by -fvisible=hidden.
 *
 * See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=20218
 *
 * Due to a more strict linker included with Xcode 4, this is disabled on Mac OS X and iOS.
 * The reason? It changes the visibility of Boost functions.  The mismatch between visibility Boost when used in the engine (default)
 * and Boost when compiled (hidden) results in mysterious link errors such as "Bad codegen, pointer diff".
 */
#   pragma GCC visibility push(default)
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include <memory>

// STL containers
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>

// Note - not in the original STL, but exists in SGI STL and STLport
// For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
#if (CM_COMPILER == CM_COMPILER_GNUC)
#   if CM_COMP_VER >= 430
#       include <tr1/unordered_map>
#       include <tr1/unordered_set> 
#   else
#       include <ext/hash_map>
#       include <ext/hash_set>
#   endif
#else
#   if (CM_COMPILER == CM_COMPILER_MSVC) && CM_COMP_VER >= 1600 // VC++ 10.0
#    	include <unordered_map>
#    	include <unordered_set>
#	else
#   	include <hash_set>
#   	include <hash_map>
#	endif
#endif 

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef __BORLANDC__
namespace CamelotEngine
{
    using namespace std;
}
#endif

extern "C" {

#   include <sys/types.h>
#   include <sys/stat.h>

}

#if CM_PLATFORM == CM_PLATFORM_WIN32
#  undef min
#  undef max
#	if !defined(NOMINMAX) && defined(_MSC_VER)
#		define NOMINMAX // required to stop windows.h messing up std::min
#	endif
#  if defined( __MINGW32__ )
#    include <unistd.h>
#  endif
#endif

#if CM_PLATFORM == CM_PLATFORM_LINUX
extern "C" {

#   include <unistd.h>
#   include <dlfcn.h>

}
#endif

#if CM_PLATFORM == CM_PLATFORM_APPLE
extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
}
#endif

#if CM_THREAD_SUPPORT
#	if !defined(NOMINMAX) && defined(_MSC_VER)
#		define NOMINMAX // required to stop windows.h messing up std::min
#	endif
#	include "CmThreadHeaders.h"
#endif

#if defined ( CM_GCC_VISIBILITY ) && (CM_PLATFORM != CM_PLATFORM_APPLE)
#   pragma GCC visibility pop
#endif

namespace CamelotEngine
{
#if CM_COMPILER == CM_COMPILER_GNUC && CM_COMP_VER >= 310
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

	// Standard containers, for easier access in my own namespace
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

	template <typename T, typename A = char > 
	struct stack 
	{ 
		typedef typename std::stack<T> type;    
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

	template <typename T, typename P = std::less<T>, typename A = char > 
	struct unordered_set 
	{ 
		typedef typename std::unordered_set<T, P> type;    
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = char > 
	struct unordered_map 
	{ 
		typedef typename std::unordered_map<K, V, P> type; 
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = char > 
	struct unordered_multimap 
	{ 
		typedef typename std::unordered_multimap<K, V, P> type; 
	}; 

	// TODO - Once VC2012 grows up and adds proper C++11 support, uncomment this
	//template <typename T, typename A = char> 
	//using deque = std::deque<T, A>;    

	//template <typename T, typename A = char >
	//using vector = std::vector<T, A>;

	//template <typename T, typename A = char > 
	//using list = std::list<T, A>;    

	//template <typename T, typename P = std::less<T>, typename A = char > 
	//using set = std::set<T, P, A>;    

	//template <typename K, typename V, typename P = std::less<K>, typename A = char > 
	//using map = std::map<K, V, P, A>; 

	//template <typename K, typename V, typename P = std::less<K>, typename A = char > 
	//using multimap = std::multimap<K, V, P, A>; 
}