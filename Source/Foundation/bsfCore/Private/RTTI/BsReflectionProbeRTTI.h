//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Renderer/BsReflectionProbe.h"
#include "Renderer/BsRenderer.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_CORE_EXPORT ReflectionProbeRTTI : public RTTIType <ReflectionProbe, IReflectable, ReflectionProbeRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFL(mTransform, 0)
			BS_RTTI_MEMBER_PLAIN(mActive, 1)
			BS_RTTI_MEMBER_PLAIN(mMobility, 2)
			BS_RTTI_MEMBER_PLAIN(mType, 3)
			BS_RTTI_MEMBER_PLAIN(mRadius, 4)
			BS_RTTI_MEMBER_PLAIN(mExtents, 5)
			BS_RTTI_MEMBER_PLAIN(mTransitionDistance, 6)
			BS_RTTI_MEMBER_REFL(mCustomTexture, 7)
			BS_RTTI_MEMBER_REFLPTR(mFilteredTexture, 8)
		BS_END_RTTI_MEMBERS
	public:
		ReflectionProbeRTTI() = default;

		void onSerializationStarted(IReflectable* obj, const UnorderedMap<String, UINT64>& params) override
		{
			ReflectionProbe* probe = static_cast<ReflectionProbe*>(obj);

			// Force the renderer task to complete, so the filtered texture is up to date
			if (probe->mRendererTask != nullptr)
				probe->mRendererTask->wait();
		}

		void onDeserializationEnded(IReflectable* obj, const UnorderedMap<String, UINT64>& params) override
		{
			// Note: Since this is a CoreObject I should call initialize() right after deserialization,
			// but since this specific type is used in Components we delay initialization until Component
			// itself does it. Keep this is mind in case this ever needs to be deserialized for non-Component
			// purposes (you'll need to call initialize manually).
		}

		const String& getRTTIName() override
		{
			static String name = "ReflectionProbe";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ReflectionProbe;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return ReflectionProbe::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}
