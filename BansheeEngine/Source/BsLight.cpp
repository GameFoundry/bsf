#include "BsLight.h"
#include "BsLightRTTI.h"

namespace BansheeEngine
{
	Light::Light(const HSceneObject& parent, LightType type, Color color,
		float luminousFlux, float range, bool castsShadows, Degree spotAngle)
		: Component(parent), mLastUpdateHash(std::numeric_limits<UINT32>::max())
	{
		mInternal = LightInternal::create(type, color, luminousFlux, range, castsShadows, spotAngle);

		setName("Light");
	}

	Light::~Light()
	{
		mInternal->destroy();
	}
	
	RTTITypeBase* Light::getRTTIStatic()
	{
		return LightRTTI::instance();
	}

	RTTITypeBase* Light::getRTTI() const
	{
		return Light::getRTTIStatic();
	}
}
