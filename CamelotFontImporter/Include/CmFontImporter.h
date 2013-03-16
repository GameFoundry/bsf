#pragma once

#include "CmFontPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace CamelotEngine
{
	class CM_FONT_EXPORT FontImporter : public SpecificImporter
	{
	public:
		FontImporter();
		virtual ~FontImporter();

		/**
		 * @brief	Should only be called by the plugin when its being loaded.
		 */
		static void startUp()
		{
			static FontImporter* importer = nullptr;
			if(importer == nullptr)
			{
				importer = new FontImporter();
				Importer::instance().registerAssetImporter(importer);
			}
		}

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual BaseResourceHandle import(const String& filePath, ConstImportOptionsPtr importOptions);

		/**
		 * @copydoc SpecificImporter::createImportOptions().
		 */
		virtual ImportOptionsPtr createImportOptions() const;
	private:
		vector<String>::type mExtensions;

		const static int MAXIMUM_TEXTURE_SIZE = 2048;
	};
}