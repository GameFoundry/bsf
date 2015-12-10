#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Renderable.
	 */
	class BS_SCR_BE_EXPORT ScriptRenderable : public ScriptObject < ScriptRenderable >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "NativeRenderable")

		/**
		 * @brief	Returns the native wrapped renderable handler.
		 */
		SPtr<Renderable> getInternal() const { return mRenderable; }

	private:
		ScriptRenderable(MonoObject* managedInstance, const HSceneObject& parentSO);
		~ScriptRenderable();

		/**
		 * @brief	Updates the internal transform of the renderable handled according to
		 *			the scene object it is attached to.
		 */
		void updateTransform(const HSceneObject& parent);

		/**
		 * @brief	Destroys the internal renderable handler object.
		 */
		void destroy();

		/**
		 * @copydoc	ScriptObject::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		SPtr<Renderable> mRenderable;
		UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* instance, ScriptSceneObject* parentSO);
		static void internal_UpdateTransform(ScriptRenderable* thisPtr, ScriptSceneObject* parentSO);
		static void internal_SetMesh(ScriptRenderable* thisPtr, ScriptMesh* mesh);
		static void internal_GetBounds(ScriptRenderable* thisPtr, ScriptSceneObject* parentSO, AABox* box, Sphere* sphere);
		static UINT64 internal_GetLayers(ScriptRenderable* thisPtr);
		static void internal_SetLayers(ScriptRenderable* thisPtr, UINT64 layers);
		static void internal_SetMaterial(ScriptRenderable* thisPtr, ScriptMaterial* material, int index);
		static void internal_SetMaterials(ScriptRenderable* thisPtr, MonoArray* materials);
		static void internal_OnDestroy(ScriptRenderable* thisPtr);
	};
}