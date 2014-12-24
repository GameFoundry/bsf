#include "BsSavedResourceData.h"
#include "BsSavedResourceDataRTTI.h"

namespace BansheeEngine
{
	SavedResourceData::SavedResourceData()
		:mAllowAsync(true)
	{ }

	SavedResourceData::SavedResourceData(const Vector<String>& dependencies, bool allowAsync)
		: mAllowAsync(allowAsync), mDependencies(dependencies)
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