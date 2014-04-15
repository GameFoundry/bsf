#pragma once

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include <memory>
#include <boost/shared_array.hpp>
#include <boost/preprocessor.hpp>

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
namespace CamelotFramework
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

namespace CamelotFramework
{
	// Standard containers, for easier access in my own namespace
	template <typename T, typename A = StdAlloc<T>> 
	struct Deque 
	{ 
		typedef typename std::deque<T, A> type;    
	}; 

	template <typename T, typename A = StdAlloc<T>> 
	struct Vector 
	{ 
		typedef typename std::vector<T, A> type;    
	}; 

	template <typename T, typename A = StdAlloc<T>> 
	struct List 
	{ 
		typedef typename std::list<T, A> type;    
	}; 

	template <typename T, typename A = StdAlloc<T>> 
	struct Stack 
	{ 
		typedef typename std::stack<T, std::deque<T, A>> type;    
	}; 

	template <typename T, typename A = StdAlloc<T>> 
	struct Queue
	{ 
		typedef typename std::queue<T, std::deque<T, A>> type;    
	}; 	

	template <typename T, typename P = std::less<T>, typename A = StdAlloc<T>> 
	struct Set 
	{ 
		typedef typename std::set<T, P, A> type;    
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = StdAlloc<std::pair<const K, V>>> 
	struct Map 
	{ 
		typedef typename std::map<K, V, P, A> type; 
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = StdAlloc<std::pair<const K, V>>> 
	struct Multimap 
	{ 
		typedef typename std::multimap<K, V, P, A> type; 
	}; 

	template <typename T, typename H = std::hash<T>, typename C = std::equal_to<T>, typename A = StdAlloc<T>> 
	struct UnorderedSet 
	{ 
		typedef typename std::unordered_set<T, H, C, A> type;    
	}; 

	template <typename K, typename V, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = StdAlloc<std::pair<const K, V>>> 
	struct UnorderedMap 
	{ 
		typedef typename std::unordered_map<K, V, H, C, A> type; 
	}; 

	template <typename K, typename V, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = StdAlloc<std::pair<const K, V>>> 
	struct UnorderedMultimap 
	{ 
		typedef typename std::unordered_multimap<K, V, H, C, A> type; 
	}; 

	//// Create a new shared pointer with a custom allocator category
	// Implementation for 0-5 parameters uses allocate_shared
#define MAKE_CM_NEW_SHARED(z, n, unused)                                     \
	template<class Type, class AllocCategory BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_shared_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::allocate_shared<Type>(StdAlloc<AllocCategory>() BOOST_PP_ENUM_TRAILING_PARAMS (n, t));     \
	}

	BOOST_PP_REPEAT_FROM_TO(0, 6, MAKE_CM_NEW_SHARED, ~)

#undef MAKE_CM_NEW_SHARED

	// Implementation for more than 5 params uses shared_ptr constructor as allocate_shared only accepts up to 5 params
#define MAKE_CM_NEW_SHARED(z, n, unused)                                     \
	template<class Type, class AllocCategory BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_shared_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::shared_ptr<Type>(cm_new<Type, AllocCategory>(BOOST_PP_ENUM_PARAMS (n, t)), &cm_delete<AllocCategory, Type>, StdAlloc<AllocCategory>());     \
	}

	BOOST_PP_REPEAT_FROM_TO(6, 15, MAKE_CM_NEW_SHARED, ~)

#undef MAKE_CM_NEW_SHARED

	//// Create a new shared pointer with a general allocator category
	// Implementation for 0-5 parameters uses allocate_shared
#define MAKE_CM_NEW_SHARED(z, n, unused)                                     \
	template<class Type BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_shared_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::allocate_shared<Type>(StdAlloc<GenAlloc>() BOOST_PP_ENUM_TRAILING_PARAMS (n, t));     \
	}

	BOOST_PP_REPEAT_FROM_TO(0, 6, MAKE_CM_NEW_SHARED, ~)

#undef MAKE_CM_NEW_SHARED

	// Implementation for more than 5 params uses shared_ptr constructor as allocate_shared only accepts up to 5 params
#define MAKE_CM_NEW_SHARED(z, n, unused)                                     \
	template<class Type BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_shared_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::shared_ptr<Type>(cm_new<Type, GenAlloc>(BOOST_PP_ENUM_PARAMS (n, t)), &cm_delete<GenAlloc, Type>, StdAlloc<GenAlloc>());     \
	}

	BOOST_PP_REPEAT_FROM_TO(6, 15, MAKE_CM_NEW_SHARED, ~)

#undef MAKE_CM_NEW_SHARED

	template<class Type, class MainAlloc>
	std::shared_ptr<Type> cm_shared_ptr(Type* data) 
	{
		return std::shared_ptr<Type>(data, &cm_delete<MainAlloc, Type>, StdAlloc<GenAlloc>());  
	}

	template<class Type, class MainAlloc, class PtrDataAlloc>
	std::shared_ptr<Type> cm_shared_ptr(Type* data) 
	{
		return std::shared_ptr<Type>(data, &cm_delete<MainAlloc, Type>, StdAlloc<PtrDataAlloc>());  
	}

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