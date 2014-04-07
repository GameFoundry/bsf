#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages the materials used by the GUI system. Ensures that the GUI system doesn't
	 * 			create an unnecessarily large amount of equivalent materials.
	 */
	class BS_EXPORT GUIMaterialManager : public CM::Module<GUIMaterialManager>
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
		const GUIMaterialInfo& requestTextMaterial(const CM::HTexture& texture, const CM::Color& tint) const;

		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture.
		 * 			
		 *			Returned material can be used for normal image rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 */
		const GUIMaterialInfo& requestImageMaterial(const CM::HTexture& texture, const CM::Color& tint) const;

		/**
		 * @brief	Releases the held reference to the material. This allows us to fully unload a material
		 * 			and their textures when they are no longer being used.
		 */
		void releaseMaterial(const GUIMaterialInfo& material) const;

		const GUIMaterialInfo* findExistingTextMaterial(const CM::HTexture& texture, const CM::Color& tint) const;
		const GUIMaterialInfo* findExistingImageMaterial(const CM::HTexture& texture, const CM::Color& tint) const;

		void forceReleaseAllMaterials();
	private:
		struct GUIMaterial
		{
			GUIMaterial()
				:refCount(0)
			{ }

			GUIMaterialInfo handle;
			CM::UINT32 refCount;
		};

		mutable CM::Vector<GUIMaterial>::type mTextMaterials;
		mutable CM::Vector<GUIMaterial>::type mImageMaterials;
	};
}