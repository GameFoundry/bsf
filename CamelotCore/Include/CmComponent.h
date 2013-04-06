#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	class CM_EXPORT Component : public IReflectable
	{
	public:
		/**
		 * @brief	Returns the GameObject this Component is assigned to.
		 */
		HGameObject gameObject() const { return mParent; }

		/**
		 * @brief	Same as gameObject(), just a shorter name.
		 */
		HGameObject GO() const { return gameObject(); }

		/**
		 * @brief	Called once per frame on all components.
		 */
		virtual void update() = 0;

		virtual ~Component();
	protected:
		friend class GameObject;

		Component(const HGameObject& parent);

		/**
		 * @brief	Destroys the Component and makes it unusable, without actually deleting it.
		 * 			
		 *			This is an internal method that should only get called by GameObject.
		 */
		void destroy();

		HGameObject mParent;
		bool mIsDestroyed;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ComponentRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		Component() {} // Serialization only
	};
}