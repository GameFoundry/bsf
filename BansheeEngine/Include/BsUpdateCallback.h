#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_EXPORT UpdateCallback : public Component
	{
	protected:
		friend class SceneObject;

		UpdateCallback(const HSceneObject& parent);
	public:
		~UpdateCallback();

		virtual void update();

		Event<void()> onUpdate;
	};
}