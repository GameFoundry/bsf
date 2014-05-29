#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages the materials used by the GUI system. Ensures that the GUI system doesn't
	 * 			create an unnecessarily large amount of equivalent materials.
	 */
	class BS_EXPORT GUIMaterialManager : public Module<GUIMaterialManager>
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
		const GUIMaterialInfo& requestTextMaterial(const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture.
		 * 			
		 *			Returned material can be used for normal image rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 */
		const GUIMaterialInfo& requestImageMaterial(const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Releases the held reference to the material. This allows us to fully unload a material
		 * 			and their textures when they are no longer being used.
		 */
		void releaseMaterial(const GUIMaterialInfo& material) const;

		const GUIMaterialInfo* findExistingTextMaterial(const HTexture& texture, const Color& tint) const;
		const GUIMaterialInfo* findExistingImageMaterial(const HTexture& texture, const Color& tint) const;

		void forceReleaseAllMaterials();
	private:
		struct GUIMaterial
		{
			GUIMaterial()
				:refCount(0)
			{ }

			GUIMaterialInfo handle;
			UINT32 refCount;
		};

		mutable Vector<GUIMaterial> mTextMaterials;
		mutable Vector<GUIMaterial> mImageMaterials;
	};
}