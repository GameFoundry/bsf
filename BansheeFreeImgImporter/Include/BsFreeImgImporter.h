#pragma once

#include "BsFreeImgPrerequisites.h"
#include "BsSpecificImporter.h"
#include "BsImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Importer implementation that handles various import for various
	 *			image formats using the FreeImg library.
	 */
	class FreeImgImporter : public SpecificImporter
	{
		struct RawImageData;

	public:
		FreeImgImporter();
		virtual ~FreeImgImporter();

		/**
		 * @copydoc	SpecificImporter::isExtensionSupported
		 */
		virtual bool isExtensionSupported(const WString& ext) const override;

		/**
		 * @copydoc	SpecificImporter::isMagicNumberSupported
		 */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const override;

		/**
		 * @copydoc	SpecificImporter::import
		 */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions) override;

		/**
		 * @copydoc SpecificImporter::createImportOptions
		 */
		virtual ImportOptionsPtr createImportOptions() const override;
	private:
		/**
		 * @brief	Converts a magic number into an extension name.
		 */
		WString magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;

		/**
		 * @brief	Imports an image from the provided data stream.
		 */
		PixelDataPtr importRawImage(DataStreamPtr fileData);

		Vector<WString> mExtensions;
		UnorderedMap<WString, int> mExtensionToFID;
	};
}