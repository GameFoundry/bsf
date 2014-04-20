#include "BsPrerequisites.h"
#include "CmComponent.h"

#include "boost/signal.hpp"

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

		boost::signal<void()> onUpdate;
	};
}