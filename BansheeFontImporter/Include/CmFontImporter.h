#pragma once

#include "CmFontPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Importer implementation that handles font import 
	 *			by using the FreeType library.
	 */
	class BS_FONT_EXPORT FontImporter : public SpecificImporter
	{
	public:
		FontImporter();
		virtual ~FontImporter();

		/**
		 * @copydoc	SpecificImporter::isExtensionSupported
		 */
		virtual bool isExtensionSupported(const WString& ext) const;

		/**
		 * @copydoc	SpecificImporter::isMagicNumberSupported
		 */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/**
		 * @copydoc	SpecificImporter::import
		 */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);

		/**
		 * @copydoc SpecificImporter::createImportOptions
		 */
		virtual ImportOptionsPtr createImportOptions() const;
	private:
		Vector<WString> mExtensions;

		const static int MAXIMUM_TEXTURE_SIZE = 2048;
	};
}