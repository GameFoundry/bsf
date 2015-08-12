#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsShader.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Shader.
	 */
	class BS_SCR_BE_EXPORT ScriptShader : public ScriptObject <ScriptShader, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Shader")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mShader; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the native internal shader resource.
		 */
		HShader getShaderHandle() const { return mShader; }
	private:
		friend class ScriptResourceManager;

		ScriptShader(MonoObject* instance, const HShader& shader);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HShader mShader;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_GetShaderParameters(ScriptShader* nativeInstance, MonoArray** outNames, MonoArray** outTypes);
	};
}