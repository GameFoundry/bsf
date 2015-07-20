#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsLightInternal.h"

namespace BansheeEngine
{
	class BS_EXPORT LightInternalRTTI : public RTTIType <LightInternal, IReflectable, LightInternalRTTI>
	{
	private:
		Vector3& getPosition(LightInternal* obj) { return obj->mPosition; }
		void setPosition(LightInternal* obj, Vector3& position) { obj->mPosition = position; }

		Quaternion& getRotation(LightInternal* obj) { return obj->mRotation; }
		void setRotation(LightInternal* obj, Quaternion& rotation) { obj->mRotation = rotation; }

		LightType& getType(LightInternal* obj) { return obj->mType; }
		void setType(LightInternal* obj, LightType& type) { obj->mType = type; }

		bool& getCastsShadow(LightInternal* obj) { return obj->mCastsShadows; }
		void setCastsShadow(LightInternal* obj, bool& castsShadow) { obj->mCastsShadows = castsShadow; }

		Color& getColor(LightInternal* obj) { return obj->mColor; }
		void setColor(LightInternal* obj, Color& color) { obj->mColor = color; }

		float& getRange(LightInternal* obj) { return obj->mRange; }
		void setRange(LightInternal* obj, float& range) { obj->mRange = range; }

		float& getIntensity(LightInternal* obj) { return obj->mIntensity; }
		void setIntensity(LightInternal* obj, float& intensity) { obj->mIntensity = intensity; }

		Degree& getSpotAngle(LightInternal* obj) { return obj->mSpotAngle; }
		void setSpotAngle(LightInternal* obj, Degree& spotAngle) { obj->mSpotAngle = spotAngle; }

		Degree& getSpotFalloffAngle(LightInternal* obj) { return obj->mSpotFalloffAngle; }
		void setSpotFalloffAngle(LightInternal* obj, Degree& spotAngle) { obj->mSpotFalloffAngle = spotAngle; }
	public:
		LightInternalRTTI()
		{
			addPlainField("mPosition", 0, &LightInternalRTTI::getPosition, &LightInternalRTTI::setPosition);
			addPlainField("mRotation", 1, &LightInternalRTTI::getRotation, &LightInternalRTTI::setRotation);
			addPlainField("mType", 2, &LightInternalRTTI::getType, &LightInternalRTTI::setType);
			addPlainField("mCastsShadow", 3, &LightInternalRTTI::getCastsShadow, &LightInternalRTTI::setCastsShadow);
			addPlainField("mColor", 4, &LightInternalRTTI::getColor, &LightInternalRTTI::setColor);
			addPlainField("mRange", 5, &LightInternalRTTI::getRange, &LightInternalRTTI::setRange);
			addPlainField("mIntensity", 6, &LightInternalRTTI::getIntensity, &LightInternalRTTI::setIntensity);
			addPlainField("mSpotAngle", 7, &LightInternalRTTI::getSpotAngle, &LightInternalRTTI::setSpotAngle);
			addPlainField("mSpotFalloffAngle", 8, &LightInternalRTTI::getSpotFalloffAngle, &LightInternalRTTI::setSpotFalloffAngle);
		}

		virtual void onDeserializationEnded(IReflectable* obj) override
		{
			LightInternal* lightInternal = static_cast<LightInternal*>(obj);
			lightInternal->initialize();
		}

		virtual const String& getRTTIName() override
		{
			static String name = "LightInternal";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_LightInternal;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return LightInternal::createEmpty();
		}
	};
}