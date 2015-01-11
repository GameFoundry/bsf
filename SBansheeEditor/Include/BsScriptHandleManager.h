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
		ScriptHandleManager(ScriptAssemblyManager& scriptObjectManager);
		~ScriptHandleManager();

	protected:
		void refreshHandles();
		void triggerHandles();
		void queueDrawCommands();

		void reloadAssemblyData();
		bool isValidHandleType(MonoClass* type, MonoClass*& componentType, MonoMethod*& ctor);

		void callPreInput(MonoObject* instance);
		void callPostInput(MonoObject* instance);
		void callDraw(MonoObject* instance);
		void callDestroy(MonoObject* instance);

		ScriptAssemblyManager& mScriptObjectManager;
		HEvent mDomainLoadConn;

		Map<String, CustomHandleData> mHandles;

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