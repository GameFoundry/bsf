//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"
#include "BsAABox.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace bs
{
	namespace ct
	{
		class RendererTask;
	}

	/** @addtogroup Implementation
	 *  @{
	 */

	/** Potential states the light probe can be in. */
	enum class LightProbeFlags
	{
		 Empty, Clean, Dirty, Removed 
	};

	/** Base class for both sim and core thread LightProbeVolume implementations. */
	class BS_CORE_EXPORT LightProbeVolumeBase
	{
	public:
		LightProbeVolumeBase();
		virtual ~LightProbeVolumeBase() { }

		/**	Returns the position of the volume, in world space. */
		Vector3 getPosition() const { return mPosition; }

		/**	Sets the position of the volume, in world space. */
		void setPosition(const Vector3& position) { mPosition = position; _markCoreDirty(); }

		/**	Returns the rotation of the volume, in world space. */
		Quaternion getRotation() const { return mRotation; }

		/**	Sets the rotation of the light, in world space. */
		void setRotation(const Quaternion& rotation) { mRotation = rotation; _markCoreDirty(); }

		/**	Checks whether the light volume should be used during rendering or not. */
		bool getIsActive() const { return mIsActive; }

		/**	Sets whether the light volume should be used during rendering or not. */
		void setIsActive(bool active) { mIsActive = active; _markCoreDirty(); }

		/** 
		 * Marks the simulation thread object as dirty and notifies the system its data should be synced with its core 
		 * thread counterpart. 
		 */
		virtual void _markCoreDirty() { }

	protected:
		Vector3 mPosition; /**< World space position. */
		Quaternion mRotation; /**< World space rotation. */
		bool mIsActive; /**< Whether the light volume should be used during rendering or not. */
	};

	/** @} */
	/** @addtogroup Renderer-Internal
	 *  @{
	 */

	namespace ct { class LightProbeVolume; }

	/** Vector representing spherical harmonic coefficients for a light probe. */
	struct LightProbeSHCoefficients
	{
		float coeffsR[9];
		float coeffsG[9];
		float coeffsB[9];
	};

	/** SH coefficients for a specific light probe, and its handle. */
	struct LightProbeCoefficientInfo
	{
		UINT32 handle;
		LightProbeSHCoefficients coefficients;
	};

	/** 
	 * Allows you to define a volume of light probes that will be used for indirect lighting. Lighting information in the
	 * scene will be interpolated from nearby probes to calculate the amount of indirect lighting at that position. It is
	 * up to the caller to place the light probes in areas where the lighting changes in order to yield the best results.
	 *
	 * The volume can never have less than 4 probes.
	 */
	class BS_CORE_EXPORT LightProbeVolume : public IReflectable, public CoreObject, public LightProbeVolumeBase
	{
		/** Internal information about a single light probe. */
		struct ProbeInfo
		{
			ProbeInfo() {}
			ProbeInfo(LightProbeFlags flags, const Vector3& position)
				:flags(flags), position(position) 
			{ }

			LightProbeFlags flags;
			Vector3 position;

			/** Coefficients are only valid directly after deserialization, or after updateCoefficients() is called. */
			LightProbeSHCoefficients coefficients;
		};
	public:
		~LightProbeVolume();

		/** Adds a new probe at the specified position and returns a handle to the probe. */
		UINT32 addProbe(const Vector3& position);

		/** Updates the position of the probe with the specified handle. */
		void setProbePosition(UINT32 handle, const Vector3& position);

		/** Retrieves the position of the probe with the specified handle. */
		Vector3 getProbePosition(UINT32 handle) const;

		/** 
		 * Removes the probe with the specified handle. Note that if this is one of the last four remaining probes in the
		 * volume it cannot be removed.
		 */
		void removeProbe(UINT32 handle);

		/**
		 * Causes the information for this specific light probe to be updated. You generally want to call this when the
		 * probe is moved or the scene around the probe changes.
		 */
		void renderProbe(UINT32 handle);

		/**
		 * Causes the information for all lights probes to be updated. You generally want to call this if you move the
		 * entire light volume or the scene around the volume changes.
		 */
		void renderProbes();

		/**	Retrieves an implementation of the object usable only from the core thread. */
		SPtr<ct::LightProbeVolume> getCore() const;

		/**
		 * Creates a new light volume with probes aligned in a grid pattern.
		 * 
		 * @param[in]	volume		Axis aligned volume to be covered by the light probes.
		 * @param[in]	density		Density of light probes in each direction. Starting on one side of the volume, a new
		 *							probe will be added every 1/density meters (per-axis). Note that one probe will be
		 *							placed at the start and at the end of the volume, regardless of density. This means the
		 *							smallest volume will have 8 probes.
		 */
		static SPtr<LightProbeVolume> create(const AABox& volume = AABox::UNIT_BOX, const Vector3& density = Vector3::ONE);
	protected:
		friend class ct::LightProbeVolume;

		LightProbeVolume(const AABox& volume, const Vector3& density);

		/** Renders the light probe data on the core thread. */
		void runRenderProbeTask();

		/** 
		 * Fetches latest SH coefficient data from the core thread. Note this method will block the caller thread until
		 * the data is fetched from the core thread. It will also force any in-progress light probe updated to finish.
		 */
		void updateCoefficients();

		/** @copydoc CoreObject::createCore */
		SPtr<ct::CoreObject> createCore() const override;

		/** @copydoc LightProbeVolumeBase::_markCoreDirty */
		void _markCoreDirty() override;

		/** @copydoc CoreObject::syncToCore */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		/**	Creates a light volume with without initializing it. Used for serialization. */
		static SPtr<LightProbeVolume> createEmpty();

	private:
		UnorderedMap<UINT32, ProbeInfo> mProbes;
		UINT32 mNextProbeId = 0;
		SPtr<ct::RendererTask> mRendererTask;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class LightProbeVolumeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		LightProbeVolume(); // Serialization only
	};

	namespace ct
	{
	/** Information about a single light probe in a light probe volume. */
	struct LightProbeInfo
	{
		/** Unique handle representing the probe. Always remains the same. */
		UINT32 handle;

		/** Flags representing the current state of the probe. */
		LightProbeFlags flags;

		/** Index into the GPU buffer where probe coefficients are stored. -1 if not assigned. Transient. */
		UINT32 bufferIdx; 
	};

	/** Core thread usable version of bs::LightProbeVolume. */
	class BS_CORE_EXPORT LightProbeVolume : public CoreObject, public LightProbeVolumeBase
	{
	public:
		~LightProbeVolume();

		/**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
		void setRendererId(UINT32 id) { mRendererId = id; }

		/**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
		UINT32 getRendererId() const { return mRendererId; }

		/** Returns a list of positions for all light probes. */
		Vector<Vector3>& getLightProbePositions() { return mProbePositions; }

		/** Populates the vector with SH coefficients for each light probe. Involves reading the GPU buffer. */
		void getProbeCoefficients(Vector<LightProbeCoefficientInfo>& output) const;
	protected:
		friend class bs::LightProbeVolume;

		LightProbeVolume(const UnorderedMap<UINT32, bs::LightProbeVolume::ProbeInfo>& probes);

		/** @copydoc CoreObject::initialize */
		void initialize() override;

		/** @copydoc CoreObject::syncToCore */
		void syncToCore(const CoreSyncData& data) override;

		/** 
		 * Renders dirty probes and updates their SH coefficients in the local GPU buffer. 
		 *
		 * @param[in]	maxProbes	Maximum number of probes to render. Set to zero to render all dirty probes. Limiting the
		 *							number of probes allows the rendering to be distributed over multiple frames.
		 * @return					True if there are no more dirty probes to process.
		 */
		bool renderProbes(UINT32 maxProbes);

		/** 
		 * Resizes the internal GPU buffer that stores light probe SH coefficients, to the specified size (in the number
		 * of probes). 
		 */
		void resizeCoefficientBuffer(UINT32 count);

		UINT32 mRendererId = 0;
		UnorderedMap<UINT32, UINT32> mProbeMap; // Map from static indices to compact list of probes
		UINT32 mFirstDirtyProbe = 0;

		Vector<Vector3> mProbePositions;
		Vector<LightProbeInfo> mProbeInfos;

		// Contains SH coefficients for the probes
		SPtr<GpuBuffer> mCoefficients;
		UINT32 mCoeffBufferSize = 0;

		// Temporary until initialization
		Vector<LightProbeSHCoefficients> mInitCoefficients;
	};
	}

	/** @} */
}