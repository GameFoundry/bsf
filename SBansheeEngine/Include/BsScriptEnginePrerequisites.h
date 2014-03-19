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
	class ManagedSerializationData;
	class ScriptSerializableAssemblyInfo;
	class ScriptSerializableObjectInfo;
	class ScriptSerializableFieldInfo;
	class ScriptSerializableFieldInfoPlain;
	class ScriptSerializableFieldInfoArray;

	enum TypeID_BansheeScript
	{
		TID_ManagedComponent = 50000,
		TID_ManagedSerializationData = 50001,
		TID_SerializableAssemblyInfo = 50004,
		TID_SerializableObjectInfo = 50005,
		TID_SerializableFieldInfo = 50006,
		TID_SerializableFieldInfoPlain = 50007,
		TID_SerializableFieldInfoArray = 50008
	};

	static const char* BansheeEngineAssemblyName = "MBansheeEngine";

	typedef std::shared_ptr<ManagedSerializationData> ManagedSerializationDataPtr;
	typedef std::shared_ptr<ScriptSerializableAssemblyInfo> ScriptSerializableAssemblyInfoPtr;
	typedef std::shared_ptr<ScriptSerializableObjectInfo> ScriptSerializableObjectInfoPtr;
	typedef std::shared_ptr<ScriptSerializableFieldInfo> ScriptSerializableFieldInfoPtr;
	typedef std::shared_ptr<ScriptSerializableFieldInfoPlain> ScriptSerializableFieldInfoPlainPtr;
	typedef std::shared_ptr<ScriptSerializableFieldInfoArray> ScriptSerializableFieldInfoArrayPtr;
}