#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class Component
	{
	private:
		friend class GameObject;

		Component(GameObjectPtr parent);
		virtual ~Component();

		/**
		 * @brief	Destroys the Component and makes it unusable, without actually deleting it.
		 * 			
		 *			This is an internal method that should only get called by GameObject.
		 */
		void destroy();

		GameObjectPtr mParent;
		bool mIsDestroyed;
	};
}