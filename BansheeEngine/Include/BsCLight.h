#pragma once

#include "BsPrerequisites.h"
#include "BsLight.h"
#include "BsComponent.h"

namespace BansheeEngine 
{
	/**
	 * @copydoc	LightInternal
	 *
	 * Wraps Light as a Component
	 */
    class BS_EXPORT CLight : public Component
    {
    public:
		CLight(const HSceneObject& parent, LightType type = LightType::Point, Color color = Color::White, 
			float intensity = 100.0f, float range = 1.0f, bool castsShadows = false, Degree spotAngle = Degree(45), 
			Degree spotFalloffAngle = Degree(40));

		virtual ~CLight();

	    /**
		 * @copydoc	LightInternal::getPosition
	     */
		Vector3 getPosition() const { return mInternal->getPosition(); }

	    /**
		 * @copydoc	LightInternal::setPosition
	     */
		void setPosition(const Vector3& position) { mInternal->setPosition(position); }

	    /**
		 * @copydoc	LightInternal::getRotation
	     */
		Quaternion getRotation() const { return mInternal->getRotation(); }

	    /**
		 * @copydoc	LightInternal::setRotation
	     */
		void setRotation(const Quaternion& rotation) { mInternal->setRotation(rotation); }

	    /**
		 * @copydoc	LightInternal::getType
	     */
		LightType getType() const { return mInternal->getType(); }

	    /**
		 * @copydoc	LightInternal::setType
	     */
		void setType(LightType type) { mInternal->setType(type); }

	    /**
		 * @copydoc	LightInternal::getCastsShadow
	     */
		bool getCastsShadow() const { return mInternal->getCastsShadow(); }

	    /**
		 * @copydoc	LightInternal::setCastsShadow
	     */
		void setCastsShadow(bool castsShadow) { mInternal->setCastsShadow(castsShadow); }

	    /**
		 * @copydoc	LightInternal::getColor
	     */
		Color getColor() const { return mInternal->getColor(); }

	    /**
		 * @copydoc	LightInternal::setColor
	     */
		void setColor(const Color& color) { mInternal->setColor(color); }

	    /**
		 * @copydoc	LightInternal::getRange
	     */
		float getRange() const { return mInternal->getRange(); }

	    /**
		 * @copydoc	LightInternal::setRange
	     */
		void setRange(float range) { mInternal->setRange(range);; }

	    /**
		 * @copydoc	LightInternal::getIntensity
	     */
		float getIntensity() const { return mInternal->getIntensity(); }

	    /**
		 * @copydoc	LightInternal::setIntensity
	     */
		void setIntensity(float intensity) { mInternal->setIntensity(intensity); }

	    /**
		 * @copydoc	LightInternal::getSpotAngle
	     */
		Degree getSpotAngle() const { return mInternal->getSpotAngle(); }

	    /**
		 * @copydoc	LightInternal::setSpotAngle
	     */
		void setSpotAngle(const Degree& spotAngle) { mInternal->setSpotAngle(spotAngle); }

	    /**
		 * @copydoc	LightInternal::getBounds
	     */
		Sphere getBounds() const;

	    /**
		 * @brief	Returns the light internals that this component wraps.
	     */
		SPtr<Light> _getInternal() const { return mInternal; }

    protected:
		mutable SPtr<Light> mInternal;

		// Only valid during construction
		LightType mType;
		Color mColor;
		float mIntensity; 
		float mRange; 
		bool mCastsShadows; 
		Degree mSpotAngle; 
		Degree mSpotFalloffAngle;

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

	    /**
		 * @copydoc	Component::onInitialized
	     */
		void onInitialized() override;

	    /**
		 * @copydoc	Component::onDestroyed
	     */
		void onDestroyed() override;

		/**
		 * @copydoc	Component::update
		 */
		virtual void update() override { }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CLightRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	protected:
		CLight() {} // Serialization only
     };
}