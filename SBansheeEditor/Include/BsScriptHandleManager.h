#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsHandleManager.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleManager : public HandleManager
	{
		struct CustomHandleData
		{
			UINT32 assemblyId;
			MonoClass* handleType;
			MonoClass* componentType;
			MonoMethod* ctor;
		};

		struct ActiveCustomHandleData
		{
			MonoObject* object;
			uint32_t gcHandle;
		};

		struct ActiveCustomHandles
		{
			HSceneObject selectedObject;
			Vector<ActiveCustomHandleData> handles;
		};

	public:
		ScriptHandleManager(const HCamera& camera, RuntimeScriptObjects& scriptObjectManager);
		~ScriptHandleManager();

		void update();

	protected:
		void refreshHandles();
		void triggerHandles();
		void queueDrawCommands();

		void reloadAssembly(MonoAssembly* assembly);
		bool isValidHandleType(MonoClass* type, MonoClass*& componentType, MonoMethod*& ctor);

		void callPreInput(MonoObject* instance);
		void callPostInput(MonoObject* instance);
		void callDraw(MonoObject* instance);
		void callDestroy(MonoObject* instance);

		RuntimeScriptObjects& mScriptObjectManager;
		HEvent mAssemblyRefreshedConn;

		Map<String, UINT32> mAssemblyNameToId;
		Map<String, CustomHandleData> mHandles;

		UINT32 mNextAssemblyId = 0;

		ActiveCustomHandles mActiveHandleData;

		MonoObject* mDefaultHandleManager = nullptr;
		uint32_t mDefaultHandleManagerGCHandle = 0;

		MonoClass* mCustomHandleAttribute = nullptr;
		MonoField* mTypeField = nullptr;
		MonoClass* mHandleBaseClass = nullptr;
		MonoClass* mDefaultHandleManagerClass = nullptr;

		typedef void(__stdcall *DestroyThunkDef) (MonoObject*, MonoException**);

		MonoMethod* mPreInputMethod;
		MonoMethod* mPostInputMethod;
		MonoMethod* mDrawMethod;
		DestroyThunkDef mDestroyThunk;
	};
}