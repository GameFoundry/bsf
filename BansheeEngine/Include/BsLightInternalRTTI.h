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

		float& getLuminousFlux(LightInternal* obj) { return obj->mLuminousFlux; }
		void setLuminousFlux(LightInternal* obj, float& luminousFlux) { obj->mLuminousFlux = luminousFlux; }

		Degree& getSpotAngle(LightInternal* obj) { return obj->mSpotAngle; }
		void setSpotAngle(LightInternal* obj, Degree& spotAngle) { obj->mSpotAngle = spotAngle; }
	public:
		LightInternalRTTI()
		{
			addPlainField("mPosition", 0, &LightInternalRTTI::getPosition, &LightInternalRTTI::setPosition);
			addPlainField("mRotation", 1, &LightInternalRTTI::getRotation, &LightInternalRTTI::setRotation);
			addPlainField("mType", 2, &LightInternalRTTI::getType, &LightInternalRTTI::setType);
			addPlainField("mCastsShadow", 3, &LightInternalRTTI::getCastsShadow, &LightInternalRTTI::setCastsShadow);
			addPlainField("mColor", 4, &LightInternalRTTI::getColor, &LightInternalRTTI::setColor);
			addPlainField("mRange", 5, &LightInternalRTTI::getRange, &LightInternalRTTI::setRange);
			addPlainField("mLuminousFlux", 6, &LightInternalRTTI::getLuminousFlux, &LightInternalRTTI::setLuminousFlux);
			addPlainField("mSpotAngle", 7, &LightInternalRTTI::getSpotAngle, &LightInternalRTTI::setSpotAngle);
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