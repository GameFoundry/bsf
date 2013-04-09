#pragma once

#include "CmFreeImgPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace CamelotEngine
{
	class FreeImgImporter : public SpecificImporter
	{
		struct RawImageData;

	public:
		FreeImgImporter();
		virtual ~FreeImgImporter();

		/**
		 * @brief	Should only be called by the plugin when its being loaded.
		 */
		static void startUp()
		{
			static FreeImgImporter* importer = nullptr;
			if(importer == nullptr)
			{
				importer = CM_NEW(FreeImgImporter, GenAlloc) FreeImgImporter();
				Importer::instance().registerAssetImporter(importer);
			}
		}

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual HResource import(const String& filePath, ConstImportOptionsPtr importOptions);
	private:
		vector<String>::type mExtensions;
		std::unordered_map<String, int> mExtensionToFID;

		String magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;
		TextureDataPtr importRawImage(DataStreamPtr fileData);
	};
}