#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRenderer.h"
#include "BsBounds.h"
#include "BsRenderableElement.h"
#include "BsSamplerOverrides.h"

namespace BansheeEngine
{
	class BeastRenderableElement;

	/**
	 * Semantics that may be used for signaling the renderer
	 * for what is a certain shader parameter used for.
	 */

	static StringID RPS_Time = "Time";
	static StringID RPS_LightDir = "LightDir";

	/**
	 * @brief	Data used by the renderer when rendering renderable handlers.
	 */
	struct RenderableData
	{
		RenderableCore* renderable;
		Vector<BeastRenderableElement> elements;
		RenderableHandler* controller;
	};

	/**
	 * @copydoc	RenderableElement
	 *
	 * Contains additional data specific to RenderBeast renderer.
	 */
	class BS_BSRND_EXPORT BeastRenderableElement : public RenderableElement
	{
	public:
		/**
		 * @brief	Optional overrides for material sampler states.
		 *			Used when renderer wants to override certain sampling
		 *			properties on a global scale (e.g. filtering most commonly).
		 */
		MaterialSamplerOverrides* samplerOverrides;

		/**
		 * @brief	Id of the owner renderable.
		 */
		UINT32 renderableId;
	};

	/**
	 * @brief	Default renderer for Banshee. Performs frustum culling, sorting and 
	 *			renders objects in custom ways determine by renderable handlers.
	 *
	 * @note	Sim thread unless otherwise noted.
	 */
	class BS_BSRND_EXPORT RenderBeast : public Renderer
	{
		/**
		 * @brief	Render data for a single render target.
		 */
		struct RenderTargetData
		{
			SPtr<RenderTargetCore> target;
			Vector<const CameraCore*> cameras;
		};

		/**
		 * @brief	Data used by the renderer for a camera.
		 */
		struct CameraData
		{
			RenderQueuePtr opaqueQueue;
			RenderQueuePtr transparentQueue;

			SPtr<RenderTargets> gbuffer;
		};

		/**
		 * @brief	Data used by the renderer for lights.
		 */
		struct LightData
		{
			LightCore* internal;
		};

	public:
		RenderBeast();
		~RenderBeast() { }

		/**
		 * @copydoc	Renderer::getName
		 */
		virtual const StringID& getName() const override;

		/**
		 * @copydoc	Renderer::renderAll
		 */
		virtual void renderAll() override;

		/**
		 * @brief	Sets options used for controlling the rendering.
		 */
		void setOptions(const SPtr<CoreRendererOptions>& options) override;

		/**
		 * @brief	Returns current set of options used for controlling the rendering.
		 */
		SPtr<CoreRendererOptions> getOptions() const override;

		/**
		 * @copydoc	Renderer::_onActivated
		 */
		virtual void _onActivated() override;

		/**
		 * @copydoc	Renderer::_onDeactivated
		 */
		virtual void _onDeactivated() override;

	private:
		/**
		 * @copydoc	Renderer::_notifyCameraAdded
		 */
		void _notifyCameraAdded(const CameraCore* camera) override;

		/**
		 * @copydoc	Renderer::_notifyCameraRemoved
		 */
		void _notifyCameraRemoved(const CameraCore* camera) override;

		/**
		 * @copydoc	Renderer::_notifyLightAdded
		 */
		void _notifyLightAdded(LightCore* light) override;

		/**
		 * @copydoc	Renderer::_notifyLightUpdated
		 */
		void _notifyLightUpdated(LightCore* light) override;

		/**
		 * @copydoc	Renderer::_notifyLightRemoved
		 */
		void _notifyLightRemoved(LightCore* light) override;

		/**
		 * @copydoc	Renderer::_notifyRenderableAdded
		 */
		void _notifyRenderableAdded(RenderableCore* renderable) override;

		/**
		 * @copydoc	Renderer::_notifyRenderableUpdated
		 */
		void _notifyRenderableUpdated(RenderableCore* renderable) override;

		/**
		 * @copydoc	Renderer::_notifyRenderableRemoved
		 */
		void _notifyRenderableRemoved(RenderableCore* renderable) override;

		/**
		 * @brief	Updates the render options on the core thread.
		 *
		 * @note	Core thread only.
		 */
		void syncRenderOptions(const RenderBeastOptions& options);

		/**
		 * @brief	Performs rendering over all camera proxies.
		 *
		 * @param	time	Current frame time in milliseconds.
		 *
		 * @note	Core thread only.
		 */
		void renderAllCore(float time);

		/**
		 * @brief	Populates camera render queues by determining visible renderable object.
		 *
		 * @param	camera	The camera to determine visibility for.
		 */
		void determineVisible(const CameraCore& camera);

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 *
		 * @param	rtData	Render target data containing the camera to render.
		 * @param	camIdx	Index of the camera to render.
		 * 					
		 * @note	Core thread only.
		 */
		void render(RenderTargetData& rtData, UINT32 camIdx);

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 *
		 * @param	camera			Camera used for determining destination render target and visibility.
		 *
		 * @note	Core thread only.
		 */
		void renderOLD(const CameraCore& camera);

		/**
		 * @brief	Creates data used by the renderer on the core thread.
		 */
		void initializeCore();

		/**
		 * @brief	Destroys data used by the renderer on the core thread.
		 */
		void destroyCore();

		/**
		 * @brief	Creates a dummy shader to be used when no other is available.
		 */
		SPtr<ShaderCore> createDefaultShader();

		/**
		 * @brief	Checks all sampler overrides in case material sampler states changed,
		 *			and updates them.
		 *
		 * @param	force	If true, all sampler overrides will be updated, regardless of a change
		 *					in the material was detected or not.
		 */
		void refreshSamplerOverrides(bool force = false);

		/**
		 * @brief	Activates the specified pass on the pipeline.
		 *
		 * @param	pass			Pass to activate.
		 * 
		 * @note	Core thread.
		 */
		static void setPass(const SPtr<PassCore>& pass);

		/**
		 * @brief	Sets parameters (textures, samplers, buffers) for the currently active pass.
		 *
		 * @param	passParams			Structure containing parameters for all stages of the pass.
		 * @param	samplerOverrides	Optional samplers to use instead of the those in the pass parameters.
		 *								Number of samplers must match number in pass parameters.						
		 * @note	Core thread.
		 */
		static void setPassParams(const SPtr<PassParametersCore>& passParams, const PassSamplerOverrides* samplerOverrides);

		Vector<RenderTargetData> mRenderTargets; // Core thread
		UnorderedMap<const CameraCore*, CameraData> mCameraData; // Core thread
		UnorderedMap<SPtr<MaterialCore>, MaterialSamplerOverrides*> mSamplerOverrides; // Core thread

		SPtr<MaterialCore> mDummyMaterial; // Core thread

		Vector<RenderableData> mRenderables; // Core thread
		Vector<Matrix4> mWorldTransforms; // Core thread
		Vector<Bounds> mWorldBounds; // Core thread

		Vector<LightData> mLights; // Core thread
		Vector<Sphere> mLightWorldBounds; // Core thread

		SPtr<RenderBeastOptions> mCoreOptions; // Core thread

		StaticRenderableHandler* mStaticHandler;
		SPtr<RenderBeastOptions> mOptions;
		bool mOptionsDirty;
	};
}