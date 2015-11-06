#pragma once

#include "BsCorePrerequisites.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Components represent primary logic elements in the scene. 
	 *			They are attached to scene objects.
	 */
	class BS_CORE_EXPORT Component : public GameObject
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
		 * @brief	Checks if this and the provided component represent the same type.
		 * 			
		 * @note	RTTI type cannot be checked directly since components can be further specialized internally 
		 * 			for scripting purposes.
		 */
		virtual bool typeEquals(const Component& other);

		/**
		 * @brief	Removes the component from parent SceneObject and deletes it. All
		 * 			the references to this component will be marked as destroyed and you
		 * 			will get an exception if you try to use them.
		 *
		 * @param	[in] immediate	If true the destruction will be performed immediately, otherwise
		 *							it will be delayed until the end of the current frame (preferred option).
		 */
		void destroy(bool immediate = false);

	protected:
		friend class SceneObject;
		friend class SceneObjectRTTI;

		Component(const HSceneObject& parent);
		virtual ~Component();

		/**
		 * @brief	Construct any resources the component needs before use. Called when the parent
		 *			scene object is instantiated. A non-instantiated component shouldn't be used for
		 *			any other purpose than serialization.
		 */
		virtual void instantiate() {}

		/**
		 * @brief	Called when the component is ready to be initialized.
		 */
		virtual void onInitialized() {}

		/**
		 * @brief	Called just before the component is destroyed.
		 */
		virtual void onDestroyed() {}

		/**
		 * @brief	Called just before the component is deactivated or destroyed.
		 */
		virtual void onDisabled() {}

		/**
		 * @brief	Called when the component is activated or created.
		 */
		virtual void onEnabled() {}

		/**
		 * @brief	Destroys this component.
		 *
		 * @param [in]	handle		Game object handle this this object.
		 * @param [in]	immediate	If true, the object will be deallocated and become unusable
		 *							right away. Otherwise the deallocation will be delayed to the end of
		 *							frame (preferred method).
		 *
		 * @note	Unlike "destroy", does not remove the component from its parent.
		 */
		void destroyInternal(GameObjectHandleBase& handle, bool immediate = false) override;
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