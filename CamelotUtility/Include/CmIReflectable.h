#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	/**
	 * @brief	Interface implemented by classes that provide run time type information.
	 */
	class CM_UTILITY_EXPORT IReflectable
	{
	public:
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
		 * @brief	INTERNAL USE. Called by each type deriving from IReflectable,
		 * 			on program load.
		 */
		static void registerDerivedClass(RTTITypeBase* derivedClass)
		{
			getDerivedClasses().push_back(derivedClass);
		}

		static std::shared_ptr<IReflectable> createInstanceFromTypeId(UINT32 rttiTypeId);
	};
}