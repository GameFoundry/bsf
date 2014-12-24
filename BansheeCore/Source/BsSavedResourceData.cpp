#include "BsSavedResourceData.h"
#include "BsSavedResourceDataRTTI.h"

namespace BansheeEngine
{
	SavedResourceData::SavedResourceData()
		:mAllowAsync(true)
	{ }

	SavedResourceData::SavedResourceData(const Vector<HResource>& dependencies, bool allowAsync)
		:mDependencies(dependencies), mAllowAsync(allowAsync)
	{

	}

	RTTITypeBase* SavedResourceData::getRTTIStatic()
	{
		return SavedResourceDataRTTI::instance();
	}

	RTTITypeBase* SavedResourceData::getRTTI() const
	{
		return SavedResourceData::getRTTIStatic();
	}
}