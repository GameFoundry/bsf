#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsPrefab.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Prefab.
	 */
	class BS_SCR_BE_EXPORT ScriptPrefab : public ScriptObject<ScriptPrefab, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Prefab")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mPrefab; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the internal wrapped prefab resource.
		 */
		HPrefab getPrefabHandle() const { return mPrefab; }
	private:
		friend class ScriptResourceManager;

		ScriptPrefab(MonoObject* instance, const HPrefab& prefab);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HPrefab mPrefab;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, ScriptSceneObject* so);
		static MonoObject* internal_Instantiate(ScriptPrefab* instance);
	};
}