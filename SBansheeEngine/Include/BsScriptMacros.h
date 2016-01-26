#pragma once

#define BS_SCRIPT_GETSET_VALUE(ParentType, Type, Name, Field)									\
	static void internal_Get##Name##(##ParentType##* nativeInstance, Type##* value)				\
	{																							\
		*value = nativeInstance->##Field;														\
	}																							\
																								\
	static void internal_Set##Name##(##ParentType##* nativeInstance, Type value)				\
	{																							\
		nativeInstance->##Field = value;															\
	}																							\

#define BS_SCRIPT_GETSET_VALUE_REF(ParentType, Type, Name, Field)								\
	static void internal_Get##Name##(##ParentType##* nativeInstance, Type##* value)				\
	{																							\
		*value = nativeInstance->##Field;														\
	}																							\
																								\
	static void internal_Set##Name##(##ParentType##* nativeInstance, Type##* value)				\
	{																							\
		nativeInstance->##Field = *value;														\
	}																							\

#define BS_SCRIPT_GETSET_OBJECT(ParentType, Type, Name, FieldNative, FieldManaged)									\
	static void internal_Get##Name##(##ParentType##* nativeInstance, MonoObject** value)							\
	{																												\
		throwIfInstancesDontMatch(nativeInstance->##FieldManaged##, &nativeInstance->##FieldNative##);				\
																													\
		if(nativeInstance->##FieldManaged != nullptr)																\
		{																											\
			*value = nativeInstance->##FieldManaged##->getManagedInstance();											\
			return;																									\
		}																											\
																													\
		*value = nullptr;																							\
	}																												\
																													\
	static void internal_Set##Name##(##ParentType##* nativeInstance, MonoObject* value)								\
	{																												\
		Type##* nativeValue = Type##::toNative(value);																\
		nativeInstance->##FieldNative = nativeValue->getInternalValue();												\
		nativeInstance->##FieldManaged = nativeValue;																\
	}

#define BS_SCRIPT_GETSET_OBJECT_SHRDPTR(ParentType, Type, Name, FieldNative, FieldManaged)							\
	static void internal_Get##Name##(##ParentType##* nativeInstance, MonoObject** value)							\
	{																												\
		throwIfInstancesDontMatch(nativeInstance->##FieldManaged##, nativeInstance->##FieldNative##.get());			\
																													\
		if(nativeInstance->##FieldManaged != nullptr)																\
		{																											\
			*value = nativeInstance->##FieldManaged##->getManagedInstance();											\
			return;																									\
		}																											\
																													\
		*value = nullptr;																							\
	}																												\
																													\
	static void internal_Set##Name##(##ParentType##* nativeInstance, MonoObject* value)								\
	{																												\
		Type##* nativeValue = Type##::toNative(value);																\
		nativeInstance->##FieldNative = nativeValue->getInternalValue();												\
		nativeInstance->##FieldManaged = nativeValue;																\
	}

#define BS_SCRIPT_SETGET_META(Type, Name)																			\
	metaData.scriptClass->addInternalCall("Internal_Get" #Name, &##Type##::internal_Get##Name##);					\
	metaData.scriptClass->addInternalCall("Internal_Set" #Name, &##Type##::internal_Set##Name##);