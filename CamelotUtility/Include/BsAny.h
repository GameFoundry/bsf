#pragma once

#include "CmPrerequisitesUtil.h"
#include <algorithm>
#include <typeinfo>

namespace BansheeEngine
{
	/**
	 * @brief	Class capable of storing any general type, and safely extracting 
	 *			the proper type from the internal data.
	 */
	class Any
	{
	private:
		class DataBase
		{
		public:
			virtual ~DataBase()
			{ }

			virtual const std::type_info& type() const = 0;
			virtual DataBase* clone() const = 0;
		};

		template <typename ValueType>
		class Data : public DataBase
		{
		public:
			Data(const ValueType& value)
				:value(value)
			{ }

			virtual const std::type_info& type() const
			{
				return typeid(ValueType);
			}

			virtual DataBase* clone() const
			{
				return new Data(value);
			}

			ValueType value;
		};

	public:
		Any() 
			:mData(0)
		{ }

		template <typename ValueType>
		Any(const ValueType& value) 
			:mData(cm_new<Data<ValueType>>(value))
		{ }

		Any(const Any& other) 
			:mData(other.mData != nullptr ? other.mData->clone() : nullptr)
		{ }

		~Any()
		{
			cm_delete(mData);
		}

		/**
		 * @brief	Swaps the contents of this object with another.
		 */
		Any& swap(Any& rhs)
		{
			std::swap(mData, rhs.mData);
			return *this;
		}

		template <typename ValueType>
		Any& operator= (const ValueType& rhs)
		{
			Any(rhs).swap(*this);
			return *this;
		}

		Any& operator= (const Any& rhs)
		{
			Any(rhs).swap(*this);
			return *this;
		}

		/**
		 * @brief	Returns true if no type is set.
		 */
		bool empty() const
		{
			return mData == nullptr;
		}

		/**
		 * @brief	Returns type of the internal data. If no internal
		 *			data is set returns void type.
		 */
		const std::type_info& type() const
		{
			return mData != nullptr ? mData->type() : typeid(void);
		}

	private:
		template <typename ValueType>
		friend ValueType* any_cast(Any*);

		template <typename ValueType>
		friend ValueType* any_cast_unsafe(Any*);

		DataBase* mData;
	};

	/**
	* @brief	Returns a pointer to the internal data of the specified type.
	*
	* @note		Will return null if cast fails.
	*/
	template <typename ValueType>
	ValueType* any_cast(Any* operand)
	{
		if (operand != nullptr && operand->type() == typeid(ValueType))
			return &static_cast<Any::Data<ValueType>*>(operand->mData)->value;
		else
			return nullptr;
	}

	/**
	 * @brief	Returns a const pointer to the internal data of the specified type.
	 *
	 * @note	Will return null if cast fails.
	 */
	template <typename ValueType>
	const ValueType* any_cast(const Any* operand)
	{
		return any_cast<ValueType>(const_cast<Any*>(operand));
	}

	/**
	* @brief	Returns a copy of the internal data of the specified type.
	*
	* @note		Throws an exception if cast fails.
	*/
	template <typename ValueType>
	ValueType any_cast(const Any& operand)
	{
		ValueType* result = any_cast<ValueType>(const_cast<Any*>(&operand));

		if (result == nullptr)
			CM_EXCEPT(InvalidStateException, "Failed to cast between Any types.");

		return *result;
	}

	/**
	 * @brief	Returns a copy of the internal data of the specified type.
	 *
	 * @note	Throws an exception if cast fails.
	 */
	template <typename ValueType>
	ValueType any_cast(Any& operand)
	{
		ValueType* result = any_cast<ValueType>(&operand);

		if (result == nullptr)
			CM_EXCEPT(InvalidStateException, "Failed to cast between Any types.");

		return *result;
	}

	/**
	* @brief	Returns a reference to the internal data of the specified type.
	*
	* @note		Throws an exception if cast fails.
	*/
	template <typename ValueType>
	const ValueType& any_cast_ref(const Any & operand)
	{
		ValueType* result = any_cast<ValueType>(const_cast<Any*>(&operand));

		if (result == nullptr)
			CM_EXCEPT(InvalidStateException, "Failed to cast between Any types.");

		return *result;
	}

	/**
	 * @brief	Returns a reference to the internal data of the specified type.
	 *
	 * @note	Throws an exception if cast fails.
	 */
	template <typename ValueType>
	ValueType& any_cast_ref(Any& operand)
	{
		ValueType* result = anyCast<ValueType>(&operand);

		if (result == nullptr)
			CM_EXCEPT(InvalidStateException, "Failed to cast between Any types.");

		return *result;
	}

	/**
	 * @brief	Casts a type without performing any kind of checks.
	 */
	template <typename ValueType>
	ValueType* any_cast_unsafe(Any* operand)
	{
		return &static_cast<Any::Data<ValueType>*>(operand->mData)->value;
	}

	/**
	* @brief	Casts a type without performing any kind of checks.
	*/
	template <typename ValueType>
	const ValueType* any_cast_unsafe(const Any* operand)
	{
		return any_cast_unsafe<ValueType>(const_cast<Any*>(operand));
	}
}