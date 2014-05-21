#pragma once

#include "CmPrerequisites.h"
#include "CmGameObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Components represent primarily logic elements in the scene. 
	 *			They are attached to scene objects.
	 */
	class CM_EXPORT Component : public GameObject
	{
	public:
		/**
		 * @brief	Returns the SceneObject this Component is assigned to.
		 */
		HSceneObject sceneObject() const { return mParent; }

		/**
		 * @copydoc	sceneObject
		 */
		HSceneObject SO() const { return sceneObject(); }

		/**
		 * @brief	Called once per frame on all components.
		 * 			
		 * @note	Internal method.
		 */
		virtual void update() { }

		/**
		 * @brief	Removes the component from parent SceneObject and deletes it. All
		 * 			the references to this component will be marked as destroyed and you
		 * 			will get an exception if you try to use them.
		 */
		void destroy();

	protected:
		friend class SceneObject;

		Component(const HSceneObject& parent);
		virtual ~Component();

		/**
		 * @brief	Called just before the component is destroyed.
		 */
		virtual void onDestroyed() {}
	private:
		Component(const Component& other) { }

	protected:
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