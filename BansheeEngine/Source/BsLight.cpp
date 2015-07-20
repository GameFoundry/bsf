#include "BsLight.h"
#include "BsLightRTTI.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	Light::Light(const HSceneObject& parent, LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		: Component(parent)
	{
		mInternal = LightInternal::create(type, color, intensity, 
			range, castsShadows, spotAngle, spotFalloffAngle);

		setName("Light");
	}

	Light::~Light()
	{
		mInternal->destroy();
	}

	Sphere Light::getBounds() const
	{
		mInternal->_updateTransform(SO());

		return mInternal->getBounds();
	}

	void Light::onInitialized()
	{
		gSceneManager()._registerLight(mInternal, sceneObject());
	}

	void Light::onDestroyed()
	{
		gSceneManager()._unregisterLight(mInternal);
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
