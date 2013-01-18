#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	/**
	 * @brief	Classes deriving from this interface need to be destroyed with the help of a destroy() method,
	 * 			before their actual destructor is called.
	 */
	class CM_UTILITY_EXPORT IDestroyable
	{
	public:
		IDestroyable();
		virtual ~IDestroyable();

		/**
		 * @brief	Destroys this object. Make sure to call this before deleting the object.
		 */
		virtual void destroy();

	private:
		bool mDestroyed;
	};
}