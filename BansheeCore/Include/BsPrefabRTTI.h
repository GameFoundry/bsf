#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsPrefab.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT PrefabRTTI : public RTTIType < Prefab, Resource, PrefabRTTI >
	{
	private:
		SceneObjectPtr getSceneObject(Prefab* obj) { return obj->mRoot.getInternalPtr(); }
		void setSceneObject(Prefab* obj, SceneObjectPtr value) { obj->mRoot = value->getHandle(); }

		UINT32& getHash(Prefab* obj) { return obj->mHash; }
		void setHash(Prefab* obj, UINT32& val) { obj->mHash = val; }

		UINT32& getNextLinkId(Prefab* obj) { return obj->mNextLinkId; }
		void setNextLinkId(Prefab* obj, UINT32& val) { obj->mNextLinkId = val; }

		String& getUUID(Prefab* obj) { return obj->mUUID; }
		void setUUID(Prefab* obj, String& val) { obj->mUUID = val; }
	public:
		PrefabRTTI()
		{
			addReflectablePtrField("mRoot", 0, &PrefabRTTI::getSceneObject, &PrefabRTTI::setSceneObject);
			addPlainField("mHash", 1, &PrefabRTTI::getHash, &PrefabRTTI::setHash);
			addPlainField("mNextLinkId", 2, &PrefabRTTI::getNextLinkId, &PrefabRTTI::setNextLinkId);
			addPlainField("mUUID", 3, &PrefabRTTI::getUUID, &PrefabRTTI::setUUID);
		}

		void onDeserializationStarted(IReflectable* ptr) override
		{
			// Make sure external IDs are broken because we do some ID matching when dealing with prefabs and keeping
			// the invalid external references could cause it to match invalid objects in case they end up having the
			// same ID.
			GameObjectManager::instance().setDeserializationMode(GODM_UseNewIds | GODM_BreakExternal);
		}

		const String& getRTTIName() override
		{
			static String name = "Prefab";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_Prefab;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return Prefab::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}