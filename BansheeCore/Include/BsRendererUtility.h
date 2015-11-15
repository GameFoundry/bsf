#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

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
		 * @brief	Draws the specified mesh.
		 *
		 * @note	Core thread.
		 */
		void draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh);

		/**
		 * @brief	Draws a quad over the entire viewport in normalized device coordinates.
		 * 			
		 * @note	Core thread.
		 */
		void drawScreenQuad(const CameraCore& camera);

	private:
		SPtr<MeshCore> mFullScreenQuadMesh;
	};

	/**
	 * @brief	Provides global access to the renderer utility.
	 * 			
	 * @note	Core thread only.
	 */
	BS_CORE_EXPORT RendererUtility& gRendererUtility();
}