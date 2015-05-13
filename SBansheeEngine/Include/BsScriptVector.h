#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptVector2 : public ScriptObject <ScriptVector2>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Vector2")

		static Vector2 unbox(MonoObject* obj);
		static MonoObject* box(const Vector2& value);

	private:
		ScriptVector2(MonoObject* instance);
	};

	class BS_SCR_BE_EXPORT ScriptVector3 : public ScriptObject <ScriptVector3>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Vector3")

		static Vector3 unbox(MonoObject* obj);
		static MonoObject* box(const Vector3& value);

	private:
		ScriptVector3(MonoObject* instance);
	};

	class BS_SCR_BE_EXPORT ScriptVector4 : public ScriptObject <ScriptVector4>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Vector4")

		static Vector4 unbox(MonoObject* obj);
		static MonoObject* box(const Vector4& value);

	private:
		ScriptVector4(MonoObject* instance);
	};
}