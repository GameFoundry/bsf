#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"
#include "CmRTTIType.h"

namespace CamelotEngine
{
	template <typename Key, typename Value>
	class KeyValuePairRTTI;

	template <typename Key, typename Value>
	class KeyValuePairBase : public IReflectable
	{
	public:
		KeyValuePairBase() {}
		KeyValuePairBase(const Key& key, const Value& value)
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
			return KeyValuePairRTTI<Key, Value>::instance();
		}

		RTTITypeBase* getRTTI() const
		{
			return KeyValuePairBase<Key, Value>::getRTTIStatic();
		}
	};

	template <typename Key, typename Value>
	class KeyValuePairRTTI : public RTTIType<KeyValuePairBase<Key, Value>, IReflectable, KeyValuePairRTTI<Key, Value>>
	{
	private:
		Key& getKey(KeyValuePairBase<Key, Value>* obj) { return obj->mKey; }
		void setKey(KeyValuePairBase<Key, Value>* obj,  Key& val) { obj->mKey = val; }

		Value& getValue(KeyValuePairBase<Key, Value>* obj) { return obj->mValue; }
		void setValue(KeyValuePairBase<Key, Value>* obj,  Value& val) { obj->mValue = val; }

	public:
		KeyValuePairRTTI()
		{
			addPlainField("mKey", 0, &KeyValuePairRTTI::getKey, &KeyValuePairRTTI::setKey);
			addPlainField("mValue", 1, &KeyValuePairRTTI::getValue, &KeyValuePairRTTI::setValue);
		}

	public:
		virtual const String& getRTTIName()
		{
			static String name = "Abstract";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Abstract;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate abstract class.");
		}
	};

	template <typename Key, typename Value, typename RTTIClass>
	class KeyValuePair : public KeyValuePairBase<Key, Value>
	{
	public:
		KeyValuePair() {}
		KeyValuePair(const Key& key, const Value& value)
			:KeyValuePairBase<Key, Value>(key, value)
		{ }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		static RTTITypeBase* getRTTIStatic()
		{
			return RTTIClass::instance();
		}

		RTTITypeBase* getRTTI() const
		{
			return KeyValuePair<Key, Value, RTTIClass>::getRTTIStatic();
		}
	};
}