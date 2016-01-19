#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"
#include "BsRect2.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Renderer-Engine
	 *  @{
	 */

	/**
	 * Contains various utility methods that make various common operations in the renderer easier.
	 * 			
	 * @note	Core thread only.
	 */
	class BS_EXPORT RendererUtility : public Module<RendererUtility>
	{
	public:
		RendererUtility();
		~RendererUtility();

		/**
		 * Activates the specified material pass for rendering. Any further draw calls will be executed using this pass.
		 *
		 * @param[in]	material	Material containing the pass.
		 * @param[in]	passIdx		Index of the pass in the material.
		 *
		 * @note	Core thread.
		 */
		void setPass(const SPtr<MaterialCore>& material, UINT32 passIdx);

		/**
		 * Sets parameters (textures, samplers, buffers) for the currently active pass.
		 *
		 * @param[in]	material	Material whose pass' parameters to bind.
		 * @param[in]	passIdx		Index of the pass in the material.
		 *					
		 * @note	Core thread.
		 */
		static void setPassParams(const SPtr<MaterialCore>& material, UINT32 passIdx = 0);

		/**
		 * Draws the specified mesh.
		 *
		 * @note	Core thread.
		 */
		void draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh);

		/**
		 * Draws a quad over the entire viewport in normalized device coordinates.
		 * 			
		 * @param[in]	viewport	Destination viewport to draw the quad in.
		 * @param[in]	uv			UV coordinates to assign to the corners of the quad.
		 * @param[in]	textureSize	Size of the texture the UV coordinates are specified for. If the UV coordinates are 
		 *							already in normalized (0, 1) range then keep this value as is. If the UV coordinates 
		 *							are in texels then set this value to the texture size so they can be normalized 
		 *							internally.
		 * 			
		 * @note	Core thread.
		 */
		void drawScreenQuad(const ViewportCore& viewport, const Rect2& uv = Rect2(0.0f, 0.0f, 1.0f, 1.0f), 
			const Vector2I& textureSize = Vector2I(1, 1));

		/** Returns a stencil mesh used for a point light (a unit sphere). */
		SPtr<MeshCore> getPointLightStencil() const { return mPointLightStencilMesh; }

		/** Returns a stencil mesh used for spot light. Actual vertex positions need to be computed in shader. */
		SPtr<MeshCore> getSpotLightStencil() const { return mSpotLightStencilMesh; }

	private:
		SPtr<MeshCore> mFullScreenQuadMesh;
		SPtr<MeshCore> mPointLightStencilMesh;
		SPtr<MeshCore> mSpotLightStencilMesh;
	};

	/**
	 * Provides easy access to RendererUtility.
	 * 			
	 * @note	Core thread only.
	 */
	BS_EXPORT RendererUtility& gRendererUtility();

	/** @} */
	/** @endcond */
}