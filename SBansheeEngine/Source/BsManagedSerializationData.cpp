#include "BsManagedSerializationData.h"
#include "BsManagedSerializationDataRTTI.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ManagedSerializationDataPtr ManagedSerializationData::create(MonoObject* managedInstance)
	{

	}

	ManagedSerializationData::ManagedSerializationData(const CM::String& ns, const CM::String& typeName)
		:mNamespace(ns), mTypename(typeName)
	{

	}

	RTTITypeBase* ManagedSerializationData::getRTTIStatic()
	{
		return ManagedSerializationDataRTTI::instance();
	}

	RTTITypeBase* ManagedSerializationData::getRTTI() const
	{
		return ManagedSerializationData::getRTTIStatic();
	}
}