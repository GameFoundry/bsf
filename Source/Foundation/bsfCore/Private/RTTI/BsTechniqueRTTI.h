//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Material/BsTechnique.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT TechniqueRTTI : public RTTIType<Technique, IReflectable, TechniqueRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			//BS_RTTI_MEMBER_PLAIN(mRenderer, 1)
			BS_RTTI_MEMBER_REFLPTR_ARRAY(mPasses, 2)
			BS_RTTI_MEMBER_PLAIN_ARRAY(mTags, 3)
			BS_RTTI_MEMBER_PLAIN(mLanguage, 4)
			BS_RTTI_MEMBER_REFL(mVariation, 5)
		BS_END_RTTI_MEMBERS

	public:
		TechniqueRTTI() = default;

		void onDeserializationEnded(IReflectable* obj, const UnorderedMap<String, UINT64>& params) override
		{
			Technique* technique = static_cast<Technique*>(obj);
			technique->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "Technique";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_Technique;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return Technique::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}
