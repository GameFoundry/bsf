#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Static class containing various utility methods that do not
	 *			fit anywhere else.
	 */
	class BS_CORE_EXPORT Utility
	{
	public:
		/**
		 * @brief	Finds all resources referenced by the specified object.
		 *
		 * @param	object		Object to search for resource dependencies.
		 * @param	recursive	Determines whether or not child objects will also be
		 *						searched (if object has any children).
		 *
		 * @returns	A list of unique, non-null resources.
		 */
		static Vector<HResource> findResourceDependencies(IReflectable& object, bool recursive = true);

	private:
		/**
		 * @brief	Helper method for for recursion when finding resource dependencies.
		 *
		 * @see	findDependencies
		 */
		static void findResourceDependenciesInternal(IReflectable& object, bool recursive, Map<String, HResource>& dependencies);
	};
}