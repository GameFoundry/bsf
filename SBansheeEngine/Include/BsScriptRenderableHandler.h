#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptRenderableHandler : public ScriptObject < ScriptRenderableHandler >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "RenderableHandler")

		SPtr<RenderableHandler> getInternal() const { return mRenderableHandler; }

	private:
		ScriptRenderableHandler(MonoObject* managedInstance, const HSceneObject& parentSO);
		~ScriptRenderableHandler();

		void updateTransform(const HSceneObject& parent);

		static void internal_Create(MonoObject* instance, ScriptSceneObject* parentSO);
		static void internal_UpdateTransform(ScriptRenderableHandler* thisPtr, ScriptSceneObject* parentSO);
		static void internal_SetMesh(ScriptRenderableHandler* thisPtr, ScriptMesh* mesh);
		static void internal_GetBounds(ScriptRenderableHandler* thisPtr, ScriptSceneObject* parentSO, AABox* box, Sphere* sphere);
		static UINT64 internal_GetLayers(ScriptRenderableHandler* thisPtr);
		static void internal_SetLayers(ScriptRenderableHandler* thisPtr, UINT64 layers);
		static void internal_SetMaterial(ScriptRenderableHandler* thisPtr, ScriptMaterial* material, int index);
		static void internal_OnDestroy(ScriptRenderableHandler* thisPtr);

		void destroy();

		SPtr<RenderableHandler> mRenderableHandler;
		UINT32 mLastUpdateHash;
	};
}