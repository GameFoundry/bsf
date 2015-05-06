#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsMaterial.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsMatrix3.h"
#include "BsMatrix4.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptMaterial : public ScriptObject <ScriptMaterial, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Material")

		HResource getNativeHandle() const { return mMaterial; }
		void setNativeHandle(const HResource& resource);

		HMaterial getMaterialHandle() const { return mMaterial; }
	private:
		friend class ScriptResourceManager;

		static void internal_CreateInstance(MonoObject* instance, ScriptShader* shader);

		static MonoObject* internal_GetShader(ScriptMaterial* nativeInstance);
		static void internal_SetShader(ScriptMaterial* nativeInstance, ScriptShader* shader);

		static void internal_SetFloat(ScriptMaterial* nativeInstance, MonoString* name, float value);
		static void internal_SetVector2(ScriptMaterial* nativeInstance, MonoString* name, Vector2 value);
		static void internal_SetVector3(ScriptMaterial* nativeInstance, MonoString* name, Vector3 value);
		static void internal_SetVector4(ScriptMaterial* nativeInstance, MonoString* name, Vector4 value);
		static void internal_SetMatrix3(ScriptMaterial* nativeInstance, MonoString* name, Matrix3 value);
		static void internal_SetMatrix4(ScriptMaterial* nativeInstance, MonoString* name, Matrix4 value);
		static void internal_SetColor(ScriptMaterial* nativeInstance, MonoString* name, Color value);
		static void internal_SetTexture2D(ScriptMaterial* nativeInstance, MonoString* name, ScriptTexture2D* value);
		static void internal_SetTexture3D(ScriptMaterial* nativeInstance, MonoString* name, ScriptTexture3D* value);
		static void internal_SetTextureCube(ScriptMaterial* nativeInstance, MonoString* name, ScriptTextureCube* value);

		static float internal_GetFloat(ScriptMaterial* nativeInstance, MonoString* name);
		static Vector2 internal_GetVector2(ScriptMaterial* nativeInstance, MonoString* name);
		static Vector3 internal_GetVector3(ScriptMaterial* nativeInstance, MonoString* name);
		static Vector4 internal_GetVector4(ScriptMaterial* nativeInstance, MonoString* name);
		static Matrix3 internal_GetMatrix3(ScriptMaterial* nativeInstance, MonoString* name);
		static Matrix4 internal_GetMatrix4(ScriptMaterial* nativeInstance, MonoString* name);
		static Color internal_GetColor(ScriptMaterial* nativeInstance, MonoString* name);
		static MonoObject* internal_GetTexture2D(ScriptMaterial* nativeInstance, MonoString* name);
		static MonoObject* internal_GetTexture3D(ScriptMaterial* nativeInstance, MonoString* name);
		static MonoObject* internal_GetTextureCube(ScriptMaterial* nativeInstance, MonoString* name);

		ScriptMaterial(MonoObject* instance, const HMaterial& material);

		void _onManagedInstanceDeleted();

		HMaterial mMaterial;
	};
}