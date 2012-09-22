#pragma once

#include "boost/static_assert.hpp"

namespace CamelotEngine
{
	template<class T>
	struct SerializableSimpleType 
	{ 
		BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, // We need this hacky check, otherwise if we only use "false" compiler will evaluate the template and trigger this message 
			"Provided data type doesn't isn't marked as serializable. Declare SerializableSimpleType template specialization to define a unique ID and needed methods for the type.");

		enum { id = T::TYPE_ID }; 
		enum { hasDynamicSize = 0 };

		void toMemory(T& data, char* memory) {}
		void fromMemory(T& data, char* memory) {}
		UINT32 getDynamicSize(T& data) {}
	};

#define CM_SERIALIZABLE_SIMPLE_TYPE(type, type_id)				\
	template<> struct SerializableSimpleType<##type##>			\
	{	enum { id=##type_id }; enum { hasDynamicSize = 0 };		\
		static void toMemory(##type##& data, char* memory)		\
		 { memcpy(memory, &data, sizeof(##type##)); }			\
		static void fromMemory(##type##& data, char* memory)	\
		 { memcpy(&data, memory, sizeof(##type##)); }			\
		static UINT32 getDynamicSize(##type##& data)			\
		{ assert(false); return sizeof(##type##); }				\
	}; 
}