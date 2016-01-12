#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRect2.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains various utility methods that make various common operations in the renderer easier.
	 * 			
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RendererUtility : public Module<RendererUtility>
	{
	public:
		RendererUtility();
		~RendererUtility();

		/**
		 * @brief	Activates the specified material pass for rendering. Any further draw calls will be executed using 
		 * 			this pass.
		 *
		 * @param	material	Material containing the pass.
		 * @param	passIdx		Index of the pass in the material.
		 *
		 * @note	Core thread.
		 */
		void setPass(const SPtr<MaterialCore>& material, UINT32 passIdx);

		/**
		 * @brief	Sets parameters (textures, samplers, buffers) for the currently active pass.
		 *
		 * @param	material	Material whose pass' parameters to bind.
		 * @param	passIdx		Index of the pass in the material.
		 *					
		 * @note	Core thread.
		 */
		static void setPassParams(const SPtr<MaterialCore>& material, UINT32 passIdx = 0);

		/**
		 * @brief	Draws the specified mesh.
		 *
		 * @note	Core thread.
		 */
		void draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh);

		/**
		 * @brief	Draws a quad over the entire viewport in normalized device coordinates.
		 * 			
		 * @param	viewport	Destination viewport to draw the quad in.
		 * @param	uv			UV coordinates to assign to the corners of the quad.
		 * @param	textureSize	Size of the texture the UV coordinates are specified for. If the UV coordinates are already
		 * 						in normalized (0, 1) range then keep this value as is. If the UV coordinates are in texels
		 * 						then set this value to the texture size so they can be normalized internally.
		 * 			
		 * @note	Core thread.
		 */
		void drawScreenQuad(const ViewportCore& viewport, const Rect2& uv = Rect2(0.0f, 0.0f, 1.0f, 1.0f), 
			const Vector2I& textureSize = Vector2I(1, 1));

	private:
		SPtr<MeshCore> mFullScreenQuadMesh;
	};

	/**
	 * @brief	Provides easy access to RendererUtility.
	 * 			
	 * @note	Core thread only.
	 */
	BS_CORE_EXPORT RendererUtility& gRendererUtility();
}