#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	class CM_EXPORT Component : public IReflectable
	{
	public:
		GameObjectPtr getGameObject() const { return mParent.lock(); }

		// TODO - This shouldn't really be public since only GameObject should be allowed to add components.
		// But then I have a problem that all derived classes need to have GameObject as a friend class.
		Component(GameObjectPtr parent);
		virtual ~Component();
	protected:
		friend class GameObject;

		/**
		 * @brief	Destroys the Component and makes it unusable, without actually deleting it.
		 * 			
		 *			This is an internal method that should only get called by GameObject.
		 */
		void destroy();

		std::weak_ptr<GameObject> mParent;
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