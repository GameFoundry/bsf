#pragma once

#include "BsPrerequisites.h"
#include "BsMaterialProxy.h"
#include "BsMeshProxy.h"
#include "BsBounds.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains all information needed for rendering a single
	 *			sub-mesh. Closely tied with Renderer.
	 */
	class BS_EXPORT RenderableElement
	{
	public:
		RenderableElement();

		/**
		 * @brief	Calculate mesh world bounds.
		 */
		Bounds calculateWorldBounds();

		/**
		 * @brief	Index of the renderable element in the global array
		 *			managed by the Renderer.
		 */
		UINT32 id;

		/**
		 * @brief	World transform matrix of the element.
		 */
		Matrix4 worldTransform;

		/**
		 * @brief	Proxy of the sub-mesh to render.
		 */
		MeshProxyPtr mesh;

		/**
		 * @brief	Proxy of the material to render the mesh with.
		 */
		MaterialProxyPtr material;

		/**
		 * @brief	Optional layer that may be used for the camera for culling
		 *			certain elements.
		 */
		UINT64 layer;

		/**
		 * @brief	Custom data that may optionally be set by the RenderableHanbdler.
		 */
		Any rendererData;

		/**
		 * @brief	Handler responsible for initializing and updating this element.
		 */
		RenderableHandler* handler;

		/**
		 * @brief	Type that determines which type of RenderableHandler to use for this element.
		 */
		RenderableType renderableType;

	private:
		bool mBoundsDirty;
	};

	/**
	 * @brief	Proxy contains data about a single Renderable object, to
	 *			be used for rendering by the Renderer on the core thread.
	 *
	 * @note	Essentially it is a snapshot of Renderable (We cannot use
	 *			Renderable itself because this is meant for the core thread).
	 */
	class BS_EXPORT RenderableProxy
	{
	public:
		~RenderableProxy();

		Vector<RenderableElement*> renderableElements;

		RenderableType renderableType;
	};
}