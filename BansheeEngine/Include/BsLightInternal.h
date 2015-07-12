#pragma once

#include "BsPrerequisites.h"
#include "BsIReflectable.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "BsColor.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Light types that determine how is light information parsed
	 *			by the renderer and other systems.
	 */
	enum class LightType
	{
		Directional, 
		Point, 
		Spot
	};

	class BS_EXPORT LightInternalBase
	{
	public:
		LightInternalBase();
		LightInternalBase(LightType type, Color color,
			float luminousFlux, float range, bool castsShadows, Degree spotAngle);

		virtual ~LightInternalBase() { }

		Vector3 getPosition() const { return mPosition; }
		void setPosition(const Vector3& position) { mPosition = position; _markCoreDirty(); }

		Quaternion getRotation() const { return mRotation; }
		void setRotation(const Quaternion& rotation) { mRotation = rotation; _markCoreDirty(); }

		LightType getType() const { return mType; }
		void setType(LightType type) { mType = type; _markCoreDirty(); }

		bool getCastsShadow() const { return mCastsShadows; }
		void setCastsShadow(bool castsShadow) { mCastsShadows = castsShadow; _markCoreDirty(); }

		Color getColor() const { return mColor; }
		void setColor(const Color& color) { mColor = color; _markCoreDirty(); }

		float getRange() const { return mRange; }
		void setRange(float range) { mRange = range; _markCoreDirty(); }

		float getLuminousFlux() const { return mLuminousFlux; }
		void setLuminousFlux(float luminousFlux) { mLuminousFlux = luminousFlux; _markCoreDirty(); }

		Degree getSpotAngle() const { return mSpotAngle; }
		void setSpotAngle(const Degree& spotAngle) { mSpotAngle = spotAngle; _markCoreDirty(); }

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		virtual void _markCoreDirty() { }

	protected:
		Vector3 mPosition; /**< World space position. */
		Quaternion mRotation; /**< World space rotation. */

		LightType mType; /**< Type of light that determines how are the rest of the parameters interpreted. */
		bool mCastsShadows; /**< Determines whether the light casts shadows. */
		Color mColor; /**< Color of the light. */
		float mRange; /**< Cut off range for the light when rendering. Visibly the light attenuation and intensity determines the range. */
		float mLuminousFlux; /**< Intensity of the light source. */
		Degree mSpotAngle; /**< Angle covered by a spot light. */
	};

	/**
	 * @copydoc	LightInternalBase
	 */
	class BS_EXPORT LightInternalCore : public CoreObjectCore, public LightInternalBase
	{
	public:
		~LightInternalCore();

	protected:
		friend class LightInternal;

		LightInternalCore(LightType type, Color color,
			float luminousFlux, float range, bool castsShadows, Degree spotAngle);

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		void initialize() override;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		void syncToCore(const CoreSyncData& data) override;
	};

	/**
	 * @copydoc	CameraHandlerBase
	 */
	class BS_EXPORT LightInternal : public IReflectable, public CoreObject, public LightInternalBase
	{
	public:
		SPtr<LightInternalCore> getCore() const;

		static SPtr<LightInternal> create(LightType type = LightType::Point, Color color = Color::White,
			float luminousFlux = 100.0f, float range = 1.0f, bool castsShadows = false, Degree spotAngle = Degree(45));

	protected:
		LightInternal(LightType type, Color color,
			float luminousFlux, float range, bool castsShadows, Degree spotAngle);

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const override;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty() override;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		static SPtr<LightInternal> createEmpty();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class LightInternalRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	protected:
		LightInternal() {} // Serialization only
	};
}