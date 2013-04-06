#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Manages the materials used by the GUI system. Ensures that the GUI system doesn't
	 * 			create an unnecessarily large amount of equivalent materials.
	 */
	class CM_EXPORT GUIMaterialManager : public Module<GUIMaterialManager>
	{
	public:
		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture.
		 * 			
		 *			Returned material can be used for text rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 */
		const HMaterial& requestTextMaterial(const HTexture& texture) const;

		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture.
		 * 			
		 *			Returned material can be used for normal image rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 */
		const HMaterial& requestImageMaterial(const HTexture& texture) const;

		/**
		 * @brief	Releases the held reference to the material. This allows us to fully unload a material
		 * 			and their textures when they are no longer being used.
		 */
		void releaseMaterial(const HMaterial& material) const;
	private:
		struct GUIMaterial
		{
			GUIMaterial()
				:refCount(0)
			{ }

			HMaterial handle;
			UINT32 refCount;
		};

		mutable vector<GUIMaterial>::type mTextMaterials;
		mutable vector<GUIMaterial>::type mImageMaterials;
	};
}