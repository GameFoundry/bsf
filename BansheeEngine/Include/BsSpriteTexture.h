#pragma once

#include "BsPrerequisites.h"
#include "BsResource.h"
#include "BsVector2.h"

namespace BansheeEngine
{
	/**
	 * @brief	Texture interface that encapsulates underlying texture which allows us
	 * 			to create a sprite texture atlas (e.g. multiple SpriteTextures referencing
	 *			different parts of a single Texture).
	 */
	class BS_EXPORT SpriteTexture : public Resource
	{
	public:
		/**
		 * @brief	Returns internal Texture that the sprite texture references.
		 */
		const HTexture& getTexture() const;

		/**
		 * @brief	Transforms wanted UV coordinates into coordinates you
		 *			can use for sampling the internal texture.
		 */
		Vector2 transformUV(const Vector2& uv) const;

		/**
		 * @brief	Returns a dummy sprite texture.
		 */
		static const HSpriteTexture& dummy();

		/**
		 * @brief	Returns width of the sprite texture in pixels.
		 */
		UINT32 getWidth() const;

		/**
		 * @brief	Returns height of the sprite texture in pixels.
		 */
		UINT32 getHeight() const;

		/**
		 * @brief	Creates a new sprite texture that references the entire area of the provided
		 *			texture.
		 */
		static HSpriteTexture create(const HTexture& texture);

		/**
		 * @brief	Creates a new sprite texture that references a sub-area of the provided
		 *			texture.
		 */
		static HSpriteTexture create(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture);

		/**
		 * @brief	Checks if the sprite texture and its internal texture have been loaded.
		 */
		static bool checkIsLoaded(const HSpriteTexture& tex);
	private:
		friend class SpriteTextureRTTI;

		/**
		 * @copydoc	create(const Vector2&, const Vector2&, const HTexture&)
		 */
		SpriteTexture(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture);

		/**
		 * @copydoc	Resource::getResourceDependencies
		 */
		void getResourceDependencies(FrameVector<HResource>& dependencies) const override;

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies) override;

		HTexture mAtlasTexture;
		Vector2 mUVOffset;
		Vector2 mUVScale;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new empty and uninitialized sprite texture. To be used by factory methods.
		 */
		static SpriteTexturePtr createEmpty();
	public:
		friend class SpriteTextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}