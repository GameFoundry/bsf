#pragma once

#include "BsMonoPrerequisites.h"

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_SCR_BE_EXPORTS
#		define BS_SCR_BE_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_SCR_BE_EXPORT
#       else
#    		define BS_SCR_BE_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define BS_SCR_BE_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_SCR_BE_EXPORT
#endif

namespace BansheeEngine
{
	class ScriptResourceManager;
	class ScriptResource;
	class ScriptFont;
	class ScriptSpriteTexture;
	class ScriptTexture2D;
	class ScriptGUIElementStyle;
	class ScriptGUIElementStateStyle;
	class ScriptGUIBase;
	class ScriptGUIArea;
	class ScriptGUILayout;
	class ScriptGUILabel;
	class ScriptGameObject;
	class ScriptSceneObject;
	class ScriptComponent;
	class ManagedComponent;
	class ScriptSerializableFieldData;
	class ScriptSerializableFieldKey;
	class ScriptSerializableFieldDataEntry;
	class ScriptSerializableTypeInfo;
	class ScriptSerializableTypeInfoPrimitive;
	class ScriptSerializableTypeInfoObject;
	class ScriptSerializableTypeInfoArray;
	class ScriptSerializableObject;
	class ScriptSerializableArray;
	class ScriptSerializableAssemblyInfo;
	class ScriptSerializableObjectInfo;
	class ScriptSerializableFieldInfo;

	enum TypeID_BansheeScript
	{
		TID_ManagedComponent = 50000,
		TID_ScriptSerializableObject = 50001,
		TID_ScriptSerializableArray = 50002,
		TID_SerializableAssemblyInfo = 50004,
		TID_SerializableObjectInfo = 50005,
		TID_SerializableFieldInfo = 50006,
		TID_SerializableTypeInfo = 50007,
		TID_SerializableTypeInfoPrimitive = 50008,
		TID_SerializableTypeInfoObject = 50009,
		TID_SerializableTypeInfoArray = 50010,
		TID_SerializableFieldData = 50011,
		TID_SerializableFieldKey = 50012,
		TID_SerializableFieldDataEntry = 50013,
		TID_SerializableFieldDataBool = 50014,
		TID_SerializableFieldDataChar = 50015,
		TID_SerializableFieldDataI8 = 50016,
		TID_SerializableFieldDataU8 = 50017,
		TID_SerializableFieldDataI16 = 50018,
		TID_SerializableFieldDataU16 = 50019,
		TID_SerializableFieldDataI32 = 50020,
		TID_SerializableFieldDataU32 = 50021,
		TID_SerializableFieldDataI64 = 50022,
		TID_SerializableFieldDataU64 = 50023,
		TID_SerializableFieldDataFloat = 50024,
		TID_SerializableFieldDataDouble = 50025,
		TID_SerializableFieldDataString = 50026,
		TID_SerializableFieldDataResourceRef = 50027,
		TID_SerializableFieldDataGameObjectRef = 50028,
		TID_SerializableFieldDataObject = 50029,
		TID_SerializableFieldDataArray = 50030
	};

	static const char* BansheeEngineAssemblyName = "MBansheeEngine";

	typedef std::shared_ptr<ScriptSerializableFieldData> ScriptSerializableFieldDataPtr;
	typedef std::shared_ptr<ScriptSerializableFieldKey> ScriptSerializableFieldKeyPtr;
	typedef std::shared_ptr<ScriptSerializableFieldDataEntry> ScriptSerializableFieldDataEntryPtr;
	typedef std::shared_ptr<ScriptSerializableTypeInfo> ScriptSerializableTypeInfoPtr;
	typedef std::shared_ptr<ScriptSerializableTypeInfoObject> ScriptSerializableTypeInfoObjectPtr;
	typedef std::shared_ptr<ScriptSerializableObject> ScriptSerializableObjectPtr;
	typedef std::shared_ptr<ScriptSerializableArray> ScriptSerializableArrayPtr;
	typedef std::shared_ptr<ScriptSerializableAssemblyInfo> ScriptSerializableAssemblyInfoPtr;
	typedef std::shared_ptr<ScriptSerializableObjectInfo> ScriptSerializableObjectInfoPtr;
	typedef std::shared_ptr<ScriptSerializableFieldInfo> ScriptSerializableFieldInfoPtr;
	typedef std::shared_ptr<ScriptSerializableTypeInfoArray> ScriptSerializableTypeInfoArrayPtr;
}