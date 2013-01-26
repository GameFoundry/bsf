#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	/**
	 * @brief	This class provides some common functionality that all low-level GPU-related objects
	 * 			need to implement.
	 * 			
	 * @note	This involves initializing, keeping track of, and releasing all GPU resources.
	 */
	class CM_EXPORT CoreGpuObject
	{
	public:
		CoreGpuObject();
		virtual ~CoreGpuObject();

		/**
		 * @brief	Destroys this object. Make sure to call this before deleting the object.
		 * 			
		 * @note	Destruction is not done immediately, and is instead just scheduled on the
		 * 			render thread. Unless called from render thread in which case it is executed right away.
		 */
		virtual void destroy();

		bool isDestroyed() { return mDestroyed; }

	protected:
		/**
		 * @brief	Frees all of the objects dynamically allocated memory. All derived classes that have something to free
		 * 			should do it here instead of their destructor. All derived classes need to call this base method when they're done.
		 */
		virtual void destroy_internal();

	private:
		bool mDestroyed;
	};
}