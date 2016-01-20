//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Utility-Core
	 *  @{
	 */

	/** Contains information about a resource dependency, including the dependant resource and number of references to it. */
	struct ResourceDependency
	{
		ResourceDependency()
			:numReferences(0)
		{ }

		HResource resource;
		UINT32 numReferences;
	};

	/** Static class containing various utility methods that do not fit anywhere else. */
	class BS_CORE_EXPORT Utility
	{
	public:
		/**
		 * Finds all resources referenced by the specified object.
		 *
		 * @param[in]	object		Object to search for resource dependencies.
		 * @param[in]	recursive	Determines whether or not child objects will also be searched (if the object has any
		 *							children).
		 * @return					A list of unique, non-null resources.
		 */
		static Vector<ResourceDependency> findResourceDependencies(IReflectable& object, bool recursive = true);

	private:
		/**
		 * Helper method for for recursion when finding resource dependencies.
		 *
		 * @see	findDependencies
		 */
		static void findResourceDependenciesInternal(IReflectable& object, bool recursive, Map<String, ResourceDependency>& dependencies);

		/**
		 * Checks if the specified type (or any of its derived classes) have any IReflectable pointer or value types as 
		 * their fields.
		 */
		static bool hasReflectableChildren(RTTITypeBase* type);
	};

	/** @} */
	/** @endcond */
}