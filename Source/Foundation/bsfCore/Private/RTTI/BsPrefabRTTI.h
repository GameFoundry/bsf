//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Scene/BsPrefab.h"
#include "Scene/BsSceneObject.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT PrefabRTTI : public RTTIType < Prefab, Resource, PrefabRTTI >
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(mHash, 1)
			//BS_RTTI_MEMBER_PLAIN(mNextLinkId, 2)
			BS_RTTI_MEMBER_PLAIN(mUUID, 3)
			BS_RTTI_MEMBER_PLAIN(mIsScene, 4)
		BS_END_RTTI_MEMBERS

		SPtr<SceneObject> getSceneObject(Prefab* obj) { return obj->mRoot.getInternalPtr(); }
		void setSceneObject(Prefab* obj, SPtr<SceneObject> value) { obj->mRoot = value->getHandle(); }

	public:
		PrefabRTTI()
		{
			addReflectablePtrField("mRoot", 0, &PrefabRTTI::getSceneObject, &PrefabRTTI::setSceneObject);
		}

		void onDeserializationStarted(IReflectable* ptr, const UnorderedMap<String, UINT64>& params) override
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

		SPtr<IReflectable> newRTTIObject() override
		{
			return Prefab::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}
