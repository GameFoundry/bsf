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
		GUIMaterialManager();

		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture, tint and group id.
		 * 			
		 *			Returned material can be used for text rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 *
		 *  @note	Group ID allows you to force the system to return different materials even if texture and tint could
		 *			otherwise be shared. Sometimes you need this if you know other material properties (like world transform)
		 *			won't be shared.
		 */
		const GUIMaterialInfo& requestTextMaterial(UINT64 groupId, const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture, tint and group id.
		 * 			
		 *			Returned material can be used for normal image rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 *
		 *  @note	Group ID allows you to force the system to return different materials even if texture and tint could
		 *			otherwise be shared. Sometimes you need this if you know other material properties (like world transform)
		 *			won't be shared.
		 */
		const GUIMaterialInfo& requestImageMaterial(UINT64 groupId, const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Creates a new material, or returns a reference to an existing one based on
		 * 			the provided primary texture, tint and group id.
		 * 			
		 *			Returned material can be used for non-transparent image rendering.
		 *			
		 *			Make sure to release all materials with a call to "releaseMaterial()".
		 *
		 * @note	Group ID allows you to force the system to return different materials even if texture and tint could
		 *			otherwise be shared. Sometimes you need this if you know other material properties (like world transform)
		 *			won't be shared.
		 */
		const GUIMaterialInfo& requestNonAlphaImageMaterial(UINT64 groupId, const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Releases the held reference to the material. This allows us to fully unload a material
		 * 			and their textures when they are no longer being used.
		 */
		void releaseMaterial(const GUIMaterialInfo& material) const;

		/**
		 * @brief	Attempts to find an existing text material with the specified color and tint. Returns null
		 *			if one cannot be found.
		 */
		const GUIMaterialInfo* findExistingTextMaterial(UINT64 groupId, const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Attempts to find an existing image material with the specified color and tint. Returns null
		 *			if one cannot be found.
		 */
		const GUIMaterialInfo* findExistingImageMaterial(UINT64 groupId, const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Attempts to find an existing non-transparent image material with the specified color and tint.
		 *			Returns null if one cannot be found.
		 */
		const GUIMaterialInfo* findExistingNonAlphaImageMaterial(UINT64 groupId, const HTexture& texture, const Color& tint) const;

		/**
		 * @brief	Releases all internal materials, whether they are used or not.
		 */
		void clearMaterials();
	private:
		/**
		 * @brief	Contains data about a GUI material and its usage count.
		 */
		struct GUIMaterial
		{
			GUIMaterial()
				:refCount(0)
			{ }

			UINT64 groupId;
			GUIMaterialInfo handle;
			UINT32 refCount;
		};

		SamplerStatePtr mGUISamplerState;

		mutable Vector<GUIMaterial> mTextMaterials;
		mutable Vector<GUIMaterial> mImageMaterials;
		mutable Vector<GUIMaterial> mNonAlphaImageMaterials;
	};
}