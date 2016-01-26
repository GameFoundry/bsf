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