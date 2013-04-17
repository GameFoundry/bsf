#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"
#include "CmRTTIType.h"

namespace CamelotFramework
{
	template <typename Key, typename Value, typename RTTIClass>
	class KeyValuePair : public IReflectable
	{
	public:
		KeyValuePair() {}
		KeyValuePair(const Key& key, const Value& value)
			:mKey(key), mValue(value)
		{ }

		Key mKey;
		Value mValue;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		static RTTITypeBase* getRTTIStatic()
		{
			return RTTIClass::instance();
		}

		virtual RTTITypeBase* getRTTI() const
		{
			return KeyValuePair<Key, Value, RTTIClass>::getRTTIStatic();
		}
	};
}