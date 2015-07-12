#pragma once

#include "BsPrerequisites.h"
#include "BsLightInternal.h"
#include "BsComponent.h"

namespace BansheeEngine 
{
	/**
	 * @copydoc	LightInternal
	 */
    class BS_EXPORT Light : public Component
    {
    public:
		Light(const HSceneObject& parent, LightType type = LightType::Point, Color color = Color::White, 
			float luminousFlux = 100.0f, float range = 1.0f, bool castsShadows = false, Degree spotAngle = Degree(45));

		virtual ~Light();

		Vector3 getPosition() const { return mInternal->getPosition(); }
		void setPosition(const Vector3& position) { mInternal->setPosition(position); }

		Quaternion getRotation() const { return mInternal->getRotation(); }
		void setRotation(const Quaternion& rotation) { mInternal->setRotation(rotation); }

		LightType getType() const { return mInternal->getType(); }
		void setType(LightType type) { mInternal->setType(type); }

		bool getCastsShadow() const { return mInternal->getCastsShadow(); }
		void setCastsShadow(bool castsShadow) { mInternal->setCastsShadow(castsShadow); }

		Color getColor() const { return mInternal->getColor(); }
		void setColor(const Color& color) { mInternal->setColor(color); }

		float getRange() const { return mInternal->getRange(); }
		void setRange(float range) { mInternal->setRange(range);; }

		float getLuminousFlux() const { return mInternal->getLuminousFlux(); }
		void setLuminousFlux(float luminousFlux) { mInternal->setLuminousFlux(luminousFlux); }

		Degree getSpotAngle() const { return mInternal->getSpotAngle(); }
		void setSpotAngle(const Degree& spotAngle) { mInternal->setSpotAngle(spotAngle); }

		SPtr<LightInternal> _getInternal() const { return mInternal; }

    protected:
		mutable SPtr<LightInternal> mInternal;
		mutable UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

	public:
		/**
		 * @copydoc	Component::update
		 */
		virtual void update() override { }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class LightRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		Light() {} // Serialization only
     };
}