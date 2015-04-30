#pragma once

#include "BsMonoPrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_SCR_BE_EXPORTS
#		define BS_SCR_BE_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_SCR_BE_EXPORT
#       else
#    		define BS_SCR_BE_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_SCR_BE_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_SCR_BE_EXPORT
#endif

namespace BansheeEngine
{
	class ScriptObjectBase;
	class ScriptResourceManager;
	class ScriptResourceBase;
	class ScriptFont;
	class ScriptSpriteTexture;
	class ScriptTexture2D;
	class ScriptTexture3D;
	class ScriptTextureCube;
	class ScriptPlainText;
	class ScriptScriptCode;
	class ScriptGUIElementStyle;
	class ScriptGUIElementStateStyle;
	class ScriptGUILayout;
	class ScriptGUILabel;
	class ScriptGameObjectBase;
	class ScriptSceneObject;
	class ScriptComponent;
	class ScriptManagedResource;
	class ScriptRenderTarget;
	class ScriptRenderTexture2D;
	class ScriptCameraHandler;
	class ManagedComponent;
	class ManagedSerializableFieldData;
	class ManagedSerializableFieldKey;
	class ManagedSerializableFieldDataEntry;
	class ManagedSerializableTypeInfo;
	class ManagedSerializableTypeInfoPrimitive;
	class ManagedSerializableTypeInfoObject;
	class ManagedSerializableTypeInfoArray;
	class ManagedSerializableTypeInfoList;
	class ManagedSerializableTypeInfoDictionary;
	class ManagedSerializableObject;
	class ManagedSerializableArray;
	class ManagedSerializableList;
	class ManagedSerializableDictionary;
	class ManagedSerializableAssemblyInfo;
	class ManagedSerializableObjectInfo;
	class ManagedSerializableFieldInfo;
	class ManagedSerializableObjectData;
	class ManagedResource;
	class ManagedResourceMetaData;
	class ScriptAssemblyManager;

	typedef GameObjectHandle<ManagedComponent> HManagedComponent;
	typedef ResourceHandle<ManagedResource> HManagedResource;

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
		TID_SerializableFieldDataArray = 50030,
		TID_SerializableFieldDataList = 50031,
		TID_SerializableFieldDataDictionary = 50032,
		TID_SerializableTypeInfoList = 50033,
		TID_SerializableTypeInfoDictionary = 50034,
		TID_ScriptSerializableList = 50035,
		TID_ScriptSerializableDictionary = 50036,
		TID_ManagedResource = 50037,
		TID_ManagedResourceMetaData = 50038,
		TID_ScriptSerializableObjectData = 50039
	};

	typedef std::shared_ptr<ManagedSerializableFieldData> ManagedSerializableFieldDataPtr;
	typedef std::shared_ptr<ManagedSerializableFieldKey> ManagedSerializableFieldKeyPtr;
	typedef std::shared_ptr<ManagedSerializableFieldDataEntry> ManagedSerializableFieldDataEntryPtr;
	typedef std::shared_ptr<ManagedSerializableTypeInfo> ManagedSerializableTypeInfoPtr;
	typedef std::shared_ptr<ManagedSerializableTypeInfoObject> ManagedSerializableTypeInfoObjectPtr;
	typedef std::shared_ptr<ManagedSerializableObject> ManagedSerializableObjectPtr;
	typedef std::shared_ptr<ManagedSerializableArray> ManagedSerializableArrayPtr;
	typedef std::shared_ptr<ManagedSerializableList> ManagedSerializableListPtr;
	typedef std::shared_ptr<ManagedSerializableDictionary> ManagedSerializableDictionaryPtr;
	typedef std::shared_ptr<ManagedSerializableAssemblyInfo> ManagedSerializableAssemblyInfoPtr;
	typedef std::shared_ptr<ManagedSerializableObjectInfo> ManagedSerializableObjectInfoPtr;
	typedef std::shared_ptr<ManagedSerializableFieldInfo> ManagedSerializableFieldInfoPtr;
	typedef std::shared_ptr<ManagedSerializableTypeInfoArray> ManagedSerializableTypeInfoArrayPtr;
	typedef std::shared_ptr<ManagedSerializableTypeInfoList> ManagedSerializableTypeInfoListPtr;
	typedef std::shared_ptr<ManagedSerializableTypeInfoDictionary> ManagedSerializableTypeInfoDictionaryPtr;
	typedef std::shared_ptr<ManagedSerializableObjectData> ManagedSerializableObjectDataPtr;
	typedef std::shared_ptr<ManagedResource> ManagedResourcePtr;
	typedef std::shared_ptr<ManagedResourceMetaData> ManagedResourceMetaDataPtr;
}