#pragma once

// TODO PORT - I define the memory allocation here because they're often used but I don't have an implementation for them. I should remove calls to the macros altogether but this will do for now

#include <malloc.h>
#include "OgreAlignedAllocator.h"

#define OGRE_NEW new
#define OGRE_DELETE delete

#define OGRE_MALLOC(bytes, category) malloc(bytes)
#define OGRE_ALLOC_T(T, count, category) static_cast<T*>(malloc(sizeof(T)*(count)))
#define OGRE_FREE(ptr, category) free((void*)ptr)

#define OGRE_NEW_T(T, category) new T
#define OGRE_NEW_ARRAY_T(T, count, category) new T[count] 
#define OGRE_DELETE_T(ptr, T, category) if(ptr){delete ptr;}
#define OGRE_DELETE_ARRAY_T(ptr, T, count, category) if(ptr){delete[] ptr;}

namespace CamelotEngine
{
	template<typename T>
	T* constructN(T* basePtr, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			new ((void*)(basePtr+i)) T();
		}
		return basePtr;
	}
}

#define OGRE_MALLOC_SIMD(bytes, category) ::CamelotEngine::AlignedMemory::allocate(bytes)
#define OGRE_MALLOC_ALIGN(bytes, category, align) ::CamelotEngine::AlignedMemory::allocate(bytes, align)
#define OGRE_ALLOC_T_SIMD(T, count, category) static_cast<T*>(::CamelotEngine::AlignedMemory::allocate(sizeof(T)*(count),CM_SIMD_ALIGNMENT))
#define OGRE_ALLOC_T_ALIGN(T, count, category, align) static_cast<T*>(::CamelotEngine::AlignedMemory::allocate(sizeof(T)*(count), align))
#define OGRE_FREE_SIMD(ptr, category) ::CamelotEngine::AlignedMemory::deallocate(ptr)
#define OGRE_FREE_ALIGN(ptr, category, align) ::CamelotEngine::AlignedMemory::deallocate(ptr)
//
#define OGRE_NEW_T_SIMD(T, category) new (::CamelotEngine::AlignedMemory::allocate(sizeof(T))) T
#define OGRE_NEW_ARRAY_T_SIMD(T, count, category) ::CamelotEngine::constructN(static_cast<T*>(::CamelotEngine::AlignedMemory::allocate(sizeof(T)*(count))), count) 
#define OGRE_DELETE_T_SIMD(ptr, T, category) if(ptr){(ptr)->~T(); ::CamelotEngine::AlignedMemory::deallocate(ptr);}
#define OGRE_DELETE_ARRAY_T_SIMD(ptr, T, count, category) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::CamelotEngine::AlignedMemory::deallocate(ptr);}
#define OGRE_NEW_T_ALIGN(T, category, align) new (::CamelotEngine::AlignedMemory::allocate(sizeof(T), align)) T
#define OGRE_NEW_ARRAY_T_ALIGN(T, count, category, align) ::CamelotEngine::constructN(static_cast<T*>(::CamelotEngine::AlignedMemory::allocate(sizeof(T)*(count), align)), count) 
#define OGRE_DELETE_T_ALIGN(ptr, T, category, align) if(ptr){(ptr)->~T(); ::CamelotEngine::AlignedMemory::deallocate(ptr);}
#define OGRE_DELETE_ARRAY_T_ALIGN(ptr, T, count, category, align) if(ptr){for (size_t _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} ::CamelotEngine::AlignedMemory::deallocate(ptr);}

namespace CamelotEngine
{
	enum MemoryCategory
	{
		/// General purpose
		MEMCATEGORY_GENERAL = 0,
		/// Geometry held in main memory
		MEMCATEGORY_GEOMETRY = 1, 
		/// Animation data like tracks, bone matrices
		MEMCATEGORY_ANIMATION = 2, 
		/// Nodes, control data
		MEMCATEGORY_SCENE_CONTROL = 3,
		/// Scene object instances
		MEMCATEGORY_SCENE_OBJECTS = 4,
		/// Other resources
		MEMCATEGORY_RESOURCE = 5,
		/// Scripting
		MEMCATEGORY_SCRIPTING = 6,
		/// Rendersystem structures
		MEMCATEGORY_RENDERSYS = 7,

		
		// sentinel value, do not use 
		MEMCATEGORY_COUNT = 8
	};
	/** @} */
	/** @} */

}