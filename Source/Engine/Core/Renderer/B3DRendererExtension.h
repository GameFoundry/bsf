//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DPrerequisites.h"

namespace b3d
{
	namespace render
	{
		struct RendererViewContext;
	}

	/** @addtogroup Renderer
	 *  @{
	 */

	/**
	 * Locations in the passes of a view at which the renderer calls RendererExtension::Render(). The passes an extension adds follow the
	 * renderer's passes of the location. render::RendererViewContext holds the attachments of the location.
	 */
	enum class RenderLocation
	{
		/** Before the view renders any scene objects. The location has no attachments. */
		Prepare,

		/** After the G-buffer is cleared, before the view renders scene objects into it. The attachments are the G-buffer and scene depth. */
		PreBasePass,

		/**
		 * After the view renders scene objects into the G-buffer, before lighting. The attachments are the G-buffer and scene depth, whose
		 * depth is read-only.
		 */
		PostBasePass,

		/**
		 * After the view renders every scene object and lights it, before post-processing. The attachments are scene color and scene
		 * depth.
		 */
		PostLightPass,

		/** After post-processing, once the view's final image is in the view's render target, which is the attachment. */
		Overlay,

		Count // Keep at end
	};

	/** Determines possible results from RendererExtension::check method. */
	enum class RendererExtensionRequest
	{
		/**
		 * Ensures RendererExtension::Render() method will be called. You want to set this to true
		 * if the internal data that's being rendered has changed since the last render() call for
		 * the specified camera.
		 */
		ForceRender,

		/**
		 * RendererExtension::Render() method will be called only if the underlying render target
		 * will change and its contents need to be redrawn. You want to set this to true if the
		 * internal data being rendered is the same as the previous frame. If your scene is fully
		 * static and nothing is changing then this will avoid doing unnecessary redrawing.
		 *
		 * Note that if any extension for the same camera and render location returns ForceRender
		 * that will dirty the render target and require all relevant extensions to be redrawn.
		 */
		RenderIfTargetDirty,

		/**
		 * RendererExtension::Render() method will not be called. e.g. use this if the camera provided
		 * is not relevant for the purposes of the extension.
		 */
		DontRender
	};

	/**
	 * Interface that can be implemented in order to provide custom rendering code to the renderer.
	 * See Renderer::addPlugin().
	 *
	 * @note	Render thread.
	 */
	class B3D_EXPORT RendererExtension
	{
	public:
		/**
		 * Creates a brand new instance of a specific implementation of a renderer extension. Queues initialization of the
		 * object on the render thread and registers it with the renderer. Destruction will be queued on the render thread when
		 * the object goes out of scope.
		 *
		 * If @p scene is non-null, the extension will be associated only with that particular scene. Otherwise it will be associated with
		 * all scenes.
		 *
		 * @note	Main thread.
		 */
		template <class T>
		static TShared<T> Create(const Any& data, const TShared<RendererScene>& scene = nullptr)
		{
			T* extension = new(B3DAllocate<T>()) T();
			QueueInitializeOnRenderThread(extension, data, scene);

			return TShared<T>(extension, &RendererExtension::QueueDeleteOnRenderThread);
		}

		/** Called when the renderer extension is first initialized. */
		virtual void Initialize(const Any& data) {}

		/** Called just before the renderer extension is destroyed. */
		virtual void Destroy() {}

		/** Returns true if the render() method should be called for the provided camera. */
		virtual RendererExtensionRequest Check(const render::Camera& camera) = 0;

		/**
		 * Called while the renderer adds the passes of the view of @p camera to its frame graph, at the extension's RenderLocation. The
		 * extension adds its passes to the graph of @p viewContext. The passes record their commands later, when the graph executes.
		 *
		 * @param	camera			Camera through which the renderer is currently rendering.
		 * @param	viewContext		Graph and attachments of the view at the extension's location.
		 */
		virtual void Render(const render::Camera& camera, const render::RendererViewContext& viewContext) = 0;

		/**
		 * Determines when will the render() method execute, compared to other plugins using the same RenderLocation.
		 * Higher number means the extension will execute before extensions with lower numbers. Priorities only matter for
		 * extensions that share the same RenderLocation.
		 */
		i32 GetPriority() const { return mPriority; }

		/**
		 * Returns a location that determines at which point in rendering should the system call the render() method. See
		 * RenderLocation.
		 */
		RenderLocation GetLocation() const { return mLocation; }

		/**	Used for sorting renderer extensions in the order they should be executed. */
		struct SortFunction
		{
			bool operator()(const RendererExtension* lhs, const RendererExtension* rhs) const;
		};

	protected:
		RendererExtension(RenderLocation location, i32 priority)
			: mLocation(location), mPriority(priority)
		{}

		virtual ~RendererExtension() = default;

	private:
		/** Initializer that triggers when a renderer extension is first constructed. */
		static void QueueInitializeOnRenderThread(RendererExtension* extension, const Any& data, const TShared<RendererScene>& scene);

		/** Deleter that triggers when a renderer extension object goes out of scope. */
		static void QueueDeleteOnRenderThread(RendererExtension* extension);

		RenderLocation mLocation;
		i32 mPriority;
		WeakSPtr<render::RendererScene> mAssociatedScene;
	};

	/** @} */
} // namespace b3d
