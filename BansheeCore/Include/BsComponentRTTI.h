#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsComponent.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT ComponentRTTI : public RTTIType<Component, GameObject, ComponentRTTI>
	{
	public:
		ComponentRTTI()
		{ }

		void onDeserializationEnded(IReflectable* obj) override
		{
			Component* comp = static_cast<Component*>(obj);
			GODeserializationData& deserializationData = any_cast_ref<GODeserializationData>(comp->mRTTIData);

			// This shouldn't be null during normal deserialization but could be during some other operations, like applying
			// a binary diff.
			if (deserializationData.ptr != nullptr)
			{
				// Register the newly created SO with the GameObjectManager and provide it with the original ID so that
				// deserialized handles pointing to this object can be resolved.
				ComponentPtr compPtr = std::static_pointer_cast<Component>(deserializationData.ptr);

				GameObjectManager::instance().registerObject(compPtr, deserializationData.originalId);
			}
			
			comp->mRTTIData = nullptr;
		}

		const String& getRTTIName() override
		{
			static String name = "Component";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_Component;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};

	/** @} */
	/** @endcond */
}