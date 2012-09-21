#pragma once

#include "CmPlatform.h"

namespace CamelotEngine
{
	typedef char INT8;
	typedef unsigned char UINT8;
	typedef short INT16;
	typedef unsigned short UINT16;
	typedef int INT32;
	typedef unsigned int UINT32;
	typedef unsigned short WCHAR;

#if CM_COMPILER == CM_COMPILER_MSVC
	typedef unsigned __int64 UINT64;
	typedef __int64 INT64;
#else
	typedef unsigned long long UINT64;
	typedef long long INT64;
#endif

#define CM_SERIALIZABLE_SIMPLE_TYPE(type, type_id) \
	template<> struct SerializableSimpleType<##type##> { enum { id=##type_id }; enum { isNativeType = 1 }; }; 
}