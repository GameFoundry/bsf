//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsIReflectable.h"
#include "CoreThread/BsCoreObject.h"
#include "Scene/BsSceneActor.h"

namespace bs
{
	namespace ct
	{
		class RendererTask;
	}

	/** @addtogroup Implementation
	 *  @{
	 */

	/**	Signals which portion of a Skybox is dirty. */
	enum class SkyboxDirtyFlag
	{
		// First few bits reserved by ActorDiryFlag
		Texture = 1 << 4
	};


	/** Base class for both core and sim thread implementations of a skybox. */
	class BS_CORE_EXPORT SkyboxBase : public SceneActor
	{
	public:
		SkyboxBase();
		virtual ~SkyboxBase() { }

		/** 
		 * Brightness multiplier that will be applied to skybox values before they're being used. Allows you to make the
		 * skybox more or less bright. Equal to one by default. 
		 */
		void setBrightness(float brightness) { mBrightness = brightness; _markCoreDirty(); }

		/** @see setBrightness */
		float getBrightness() const { return mBrightness; }

	protected:
		float mBrightness; /**< Multiplier to apply to evaluated skybox values before using them. */
	};

	/** @} */
	/** @addtogroup Renderer-Internal
	 *  @{
	 */

	namespace ct { class Skybox; }

	/** Allows you to specify an environment map to use for sampling radiance of the sky. */
	class BS_CORE_EXPORT Skybox : public IReflectable, public CoreObject, public SkyboxBase
	{
	public:
		~Skybox();

		/**
		 * Determines an environment map to use for sampling skybox radiance. Must be a cube-map texture, and should ideally
		 * contain HDR data.
		 */
		void setTexture(const HTexture& texture);

		/** @copydoc setTexture */
		HTexture getTexture() const { return mTexture; }
		
		/**	Retrieves an implementation of the skybox usable only from the core thread. */
		SPtr<ct::Skybox> getCore() const;

		/** Creates a new skybox. */
		static SPtr<Skybox> create();

	protected:
		Skybox();

		/** 
		 * Filters the skybox radiance texture, generating filtered radiance (for reflections) and irradiance. Should be 
		 * called any time the skybox texture changes. 
		 */
		void filterTexture();

		/** @copydoc CoreObject::createCore */
		SPtr<ct::CoreObject> createCore() const override;

		/** @copydoc SkyboxBase::_markCoreDirty */
		void _markCoreDirty(ActorDirtyFlag flags = ActorDirtyFlag::Everything) override;

		/** @copydoc CoreObject::syncToCore */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		HTexture mTexture;
		SPtr<Texture> mFilteredRadiance;
		SPtr<Texture> mIrradiance;
		SPtr<ct::RendererTask> mRendererTask;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SkyboxRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

		/**	Creates a new skybox instance without initializing it. */
		static SPtr<Skybox> createEmpty();
	};

	namespace ct
	{
		/** Core thread usable version of a bs::Skybox */
		class BS_CORE_EXPORT Skybox : public CoreObject, public SkyboxBase
		{
		public:
			~Skybox();

			/** @copydoc bs::Skybox::setTexture */
			SPtr<Texture> getTexture() const { return mTexture; }
		
			/** 
			 * Returns a texture containing filtered version of the radiance texture used for reflections. This might not
			 * be available if it hasn't been generated yet.
			 */
			SPtr<Texture> getFilteredRadiance() const { return mFilteredRadiance; }

			/**
			 * Returns a texture containing sky irradiance. This might not be available if it hasn't been generated yet.
			 */
			SPtr<Texture> getIrradiance() const { return mIrradiance; }

		protected:
			friend class bs::Skybox;

			Skybox(const SPtr<Texture>& radiance, const SPtr<Texture>& filteredRadiance, const SPtr<Texture>& irradiance);

			/** @copydoc CoreObject::initialize */
			void initialize() override;

			/** @copydoc CoreObject::syncToCore */
			void syncToCore(const CoreSyncData& data) override;

			SPtr<Texture> mTexture;
			SPtr<Texture> mFilteredRadiance;
			SPtr<Texture> mIrradiance;
		};
	}

	/** @} */
}
