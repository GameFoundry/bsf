#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsLight.h"

namespace BansheeEngine
{
	class BS_EXPORT LightRTTI : public RTTIType <Light, IReflectable, LightRTTI>
	{
	private:
		Vector3& getPosition(Light* obj) { return obj->mPosition; }
		void setPosition(Light* obj, Vector3& position) { obj->mPosition = position; }

		Quaternion& getRotation(Light* obj) { return obj->mRotation; }
		void setRotation(Light* obj, Quaternion& rotation) { obj->mRotation = rotation; }

		LightType& getType(Light* obj) { return obj->mType; }
		void setType(Light* obj, LightType& type) { obj->mType = type; }

		bool& getCastsShadow(Light* obj) { return obj->mCastsShadows; }
		void setCastsShadow(Light* obj, bool& castsShadow) { obj->mCastsShadows = castsShadow; }

		Color& getColor(Light* obj) { return obj->mColor; }
		void setColor(Light* obj, Color& color) { obj->mColor = color; }

		float& getRange(Light* obj) { return obj->mRange; }
		void setRange(Light* obj, float& range) { obj->mRange = range; }

		float& getIntensity(Light* obj) { return obj->mIntensity; }
		void setIntensity(Light* obj, float& intensity) { obj->mIntensity = intensity; }

		Degree& getSpotAngle(Light* obj) { return obj->mSpotAngle; }
		void setSpotAngle(Light* obj, Degree& spotAngle) { obj->mSpotAngle = spotAngle; }

		Degree& getSpotFalloffAngle(Light* obj) { return obj->mSpotFalloffAngle; }
		void setSpotFalloffAngle(Light* obj, Degree& spotAngle) { obj->mSpotFalloffAngle = spotAngle; }
	public:
		LightRTTI()
		{
			addPlainField("mPosition", 0, &LightRTTI::getPosition, &LightRTTI::setPosition);
			addPlainField("mRotation", 1, &LightRTTI::getRotation, &LightRTTI::setRotation);
			addPlainField("mType", 2, &LightRTTI::getType, &LightRTTI::setType);
			addPlainField("mCastsShadow", 3, &LightRTTI::getCastsShadow, &LightRTTI::setCastsShadow);
			addPlainField("mColor", 4, &LightRTTI::getColor, &LightRTTI::setColor);
			addPlainField("mRange", 5, &LightRTTI::getRange, &LightRTTI::setRange);
			addPlainField("mIntensity", 6, &LightRTTI::getIntensity, &LightRTTI::setIntensity);
			addPlainField("mSpotAngle", 7, &LightRTTI::getSpotAngle, &LightRTTI::setSpotAngle);
			addPlainField("mSpotFalloffAngle", 8, &LightRTTI::getSpotFalloffAngle, &LightRTTI::setSpotFalloffAngle);
		}

		virtual void onDeserializationEnded(IReflectable* obj) override
		{
			// Note: Since this is a CoreObject I should call initialize() right after deserialization,
			// but since this specific type is used in Components we delay initialization until Component
			// itself does it. Keep this is mind in case this ever needs to be deserialized for non-Component 
			// purposes (you'll need to call initialize manually).
		}

		virtual const String& getRTTIName() override
		{
			static String name = "Light";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_Light;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return Light::createEmpty();
		}
	};
}