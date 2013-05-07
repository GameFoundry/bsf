#include "BsPrerequisites.h"
#include "CmComponent.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT UpdateCallback : public CM::Component
	{
	protected:
		friend class CM::SceneObject;

		UpdateCallback(const CM::HSceneObject& parent);
	public:
		~UpdateCallback();

		virtual void update();

		boost::signal<void()> onUpdate;
	};
}