#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	/**
	 * @brief	Interface implemented by classes that provide run time type information.
	 * 			It just provides a method that returns an interface to the class metadata.
	 */
	class CM_UTILITY_EXPORT IReflectable
	{
	public:
		/**
		 * @brief	Returns an interface you can use to access class Run Time Type Information.
		 *
		 * @note	It is suggested you derive your own version of RTTIType, in which you
		 * 			may encapsulate all reflection specific operations. Although this is optional.
		 */
		virtual RTTITypeBase* getRTTI() const = 0;

		/**
		 * @brief	Creates an empty instance of a reflectable object. This is a bit of a hack, 
		 * 			but EVERY class deriving from IReflectable needs to implement this method. 
		 * 			You will get a compiler error otherwise.
		 */
		static IReflectable* newObject();
	};
}