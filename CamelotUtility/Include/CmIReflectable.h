#pragma once

#include "CmPrerequisitesUtil.h"
#include <boost/any.hpp>

namespace CamelotFramework
{
	/**
	 * @brief	Interface implemented by classes that provide run time type information.
	 */
	class CM_UTILITY_EXPORT IReflectable
	{
	public:
		virtual ~IReflectable() {}

		/**
		 * @brief	Returns an interface you can use to access class Run Time Type Information.
		 *
		 * @note	You must derive your own version of RTTIType, in which you
		 * 			may encapsulate all reflection specific operations. 
		 */
		virtual RTTITypeBase* getRTTI() const = 0;

		/**
		 * @brief	Returns all classes deriving directly from IReflectable.
		 * 			You can call another version of this method on the returned type class,
		 * 			to find classes deeper in the hierarchy.
		 */
		static vector<RTTITypeBase*>::type& getDerivedClasses()
		{
			static vector<RTTITypeBase*>::type mRTTIDerivedClasses;
			return mRTTIDerivedClasses;
		}

		/**
		 * @brief	INTERNAL USE ONLY. Called by each type deriving from IReflectable,
		 * 			on program load.
		 */
		static void registerDerivedClass(RTTITypeBase* derivedClass);
		static std::shared_ptr<IReflectable> createInstanceFromTypeId(UINT32 rttiTypeId);
		static RTTITypeBase* getRTTIfromTypeId(UINT32 rttiTypeId);
		static bool isTypeIdDuplicate(UINT32 typeId);

		/**
		 * @brief	Returns true if current RTTI class is derived from "base".
		 * 			(Or if it is the same type as base)
		 */
		bool isDerivedFrom(RTTITypeBase* base);

		UINT32 getTypeId() const;
		const String& getTypeName() const;
	protected:
		boost::any mRTTIData; // Temporary per-instance data storage used during various RTTI operations.
							  // Needed since there is one RTTI class instance per type and sometimes we need per-instance data.
	};
}