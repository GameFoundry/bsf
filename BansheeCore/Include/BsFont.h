#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"
#include "BsFontDesc.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains data about font characters of a specific size. Contains
	 *			textures in which the characters are located and information
	 *			about each individual character.
	 */
	struct BS_CORE_EXPORT FontData : public IReflectable
	{
		/**
		 * @brief	Returns a character description for the character with the specified ID.
		 */
		const CHAR_DESC& getCharDesc(UINT32 charId) const;

		UINT32 size; /**< Font size for which the data is contained. */
		FONT_DESC fontDesc; /**< Font description containing per-character and general font data. */
		Vector<HTexture> texturePages; /**< Textures in which the characters are stored. */

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class FontDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	// TODO - When saved on disk font currently stores a copy of the texture pages. This should be acceptable
	// if you import a new TrueType or OpenType font since the texture will be generated on the spot
	// but if you use a bitmap texture to initialize the font manually, then you will potentially have duplicate textures.
	// Also, changing the source texture will not automatically update the font because there is no direct link between them.
	// -- This is probably not a large problem, but it is something to keep an eye out.

	/**
	 * @brief	Font resource containing data about textual characters
	 *			and how to render text.
	 */
	class BS_CORE_EXPORT Font : public Resource
	{
	public:
		virtual ~Font();

		/**
		 * @brief	Initializes the font with specified per-size font data.
		 *
		 * @note	Internal method. Factory methods will call this automatically for you.
		 */
		void initialize(const Vector<FontData>& fontData);

		/**
		 * @brief	Returns font data for a specific size if it exists, null otherwise.
		 */
		const FontData* getFontDataForSize(UINT32 size) const;

		/**
		 * @brief	Attempts to find nearest available size next to the provided size.
		 */
		INT32 getClosestAvailableSize(UINT32 size) const;

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new font from the provided per-size font data.
		 */
		static HFont create(const Vector<FontData>& fontInitData);

		/**
		 * @brief	Creates a new font pointer.
		 *
		 * @note	Internal method.
		 */
		static FontPtr _createPtr(const Vector<FontData>& fontInitData);

	protected:
		friend class FontManager;

		Font();

		/**
		 * @copydoc	Resource::getResourceDependencies
		 */
		void getResourceDependencies(FrameVector<HResource>& dependencies) const override;

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies) override;

	private:
		Map<UINT32, FontData> mFontDataPerSize;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class FontRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}