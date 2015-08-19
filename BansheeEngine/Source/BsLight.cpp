#include "BsLight.h"
#include "BsLightRTTI.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	Light::Light(const HSceneObject& parent, LightType type, Color color,
		float intensity, float range, bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
		: Component(parent), mType(type), mColor(color), mIntensity(intensity), mRange(range),
		mCastsShadows(castsShadows), mSpotAngle(spotAngle), mSpotFalloffAngle(spotFalloffAngle)
	{
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
		// If mInternal already exists this means this object was deserialized,
		// so all we need to do is initialize it.
		if (mInternal != nullptr)
			mInternal->initialize();
		else
		{
			mInternal = LightInternal::create(mType, mColor, mIntensity,
				mRange, mCastsShadows, mSpotAngle, mSpotFalloffAngle);
		}

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
