#pragma once

#include "CmPrerequisites.h"
#include "CmGameObject.h"

namespace CamelotFramework
{
	class CM_EXPORT Component : public GameObject
	{
	public:
		/**
		 * @brief	Returns the SceneObject this Component is assigned to.
		 */
		HSceneObject sceneObject() const { return mParent; }

		/**
		 * @brief	Same as sceneObject(), just a shorter name.
		 */
		HSceneObject SO() const { return sceneObject(); }

		/**
		 * @brief	Called once per frame on all components.
		 */
		virtual void update() = 0;

	protected:
		friend class SceneObject;

		Component(const HSceneObject& parent);
		virtual ~Component();

		HSceneObject mParent;

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