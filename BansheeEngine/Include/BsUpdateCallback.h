//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Component that allows external objects to subscribe to
	 *			the standard Component update method.
	 */
	class BS_EXPORT UpdateCallback : public Component
	{
	public:
		~UpdateCallback();

		/**
		 * @copydoc	Component::update
		 */
		virtual void update();

		/**
		 * @brief	Called whenever update() is called.
		 */
		Event<void()> onUpdate;

	protected:
		friend class SceneObject;

		UpdateCallback(const HSceneObject& parent);
	};
}