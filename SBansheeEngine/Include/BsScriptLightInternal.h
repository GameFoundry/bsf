#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "BsDegree.h"
#include "BsColor.h"
#include "BsLightInternal.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for LightInternal.
	 */
	class BS_SCR_BE_EXPORT ScriptLightInternal : public ScriptObject <ScriptLightInternal>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "LightInternal")

		/**
		 * @brief	Gets the wrapped native LightInternal object.
		 */
		SPtr<LightInternal> getInternal() const { return mLightInternal; }

	private:
		ScriptLightInternal(MonoObject* managedInstance, const HSceneObject& parentSO);
		~ScriptLightInternal();

		SPtr<LightInternal> mLightInternal;
		UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_create(MonoObject* managedInstance, ScriptSceneObject* parentSO);

		static Vector3 internal_getPosition(ScriptLightInternal* thisPtr);
		static void internal_setPosition(ScriptLightInternal* thisPtr, Vector3 position);

		static Quaternion internal_getRotation(ScriptLightInternal* thisPtr);
		static void internal_setRotation(ScriptLightInternal* thisPtr, Quaternion rotation);

		static LightType internal_getType(ScriptLightInternal* thisPtr);
		static void internal_setType(ScriptLightInternal* thisPtr, LightType type);

		static bool internal_getCastsShadow(ScriptLightInternal* thisPtr);
		static void internal_setCastsShadow(ScriptLightInternal* thisPtr, bool castsShadow);

		static Color internal_getColor(ScriptLightInternal* thisPtr);
		static void internal_setColor(ScriptLightInternal* thisPtr, Color color);

		static float internal_getRange(ScriptLightInternal* thisPtr);
		static void internal_setRange(ScriptLightInternal* thisPtr, float range);

		static float internal_getIntensity(ScriptLightInternal* thisPtr);
		static void internal_setIntensity(ScriptLightInternal* thisPtr, float intensity);

		static Degree internal_getSpotAngle(ScriptLightInternal* thisPtr);
		static void internal_setSpotAngle(ScriptLightInternal* thisPtr, Degree spotAngle);

		static Degree internal_getSpotFalloffAngle(ScriptLightInternal* thisPtr);
		static void internal_setSpotFalloffAngle(ScriptLightInternal* thisPtr, Degree spotFalloffAngle);

		static Sphere internal_getBounds(ScriptLightInternal* thisPtr);

		static void internal_updateTransform(ScriptLightInternal* thisPtr, ScriptSceneObject* parent);
		static void internal_onDestroy(ScriptLightInternal* instance);
	};
}