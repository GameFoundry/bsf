#pragma once

#define CM_SCRIPT_GETSET_VALUE(ParentType, Type, Name, Field)									\
	void internal_Get##Name##(##ParentType##* nativeInstance, Type##* value)					\
	{																							\
		*value = nativeInstance->##Field;														\
	}																							\
																								\
	void internal_Set##Name##(##ParentType##* nativeInstance, Type value)						\
	{																							\
		nativeInstance->##Field = value;															\
	}																							\

#define CM_SCRIPT_GETSET_REF_VALUE(ParentType, Type, Name, Field)								\
	void internal_Get##Name##(##ParentType##* nativeInstance, Type##* value)					\
	{																							\
		*value = nativeInstance->##Field;														\
	}																							\
																								\
	void internal_Set##Name##(##ParentType##* nativeInstance, Type* value)						\
	{																							\
		nativeInstance->##Field = *value;														\
	}																							\

#define CM_SCRIPT_GETSET_OBJECT(ParentType, Type, Name, FieldNative, FieldNativeRaw, FieldManaged)					\
	void internal_Get##Name##(##ParentType##* nativeInstance, MonoObject** value)									\
	{																												\
		throwIfInstancesDontMatch(nativeInstance->##FieldManaged##, nativeInstance->##FieldNativeRaw##);				\
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
	void internal_Set##Name##(##ParentType##* nativeInstance, MonoObject* value)									\
	{																												\
		Type##* nativeValue = Type##::toNative(value);																\
		nativeInstance->##FieldNative = nativeValue->getInternalValue();												\
	}																												

#define CM_SCRIPT_SETGET_META(Type, Name)																			\
	metaData.scriptClass->addInternalCall("Internal_Get##Name##", &##Type##::internal_Get##Name##);