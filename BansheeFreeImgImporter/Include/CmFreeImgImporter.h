#pragma once

#include "CmFreeImgPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace BansheeEngine
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
				importer = cm_new<FreeImgImporter>();
				Importer::instance()._registerAssetImporter(importer);
			}
		}

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);
	private:
		Vector<WString> mExtensions;
		UnorderedMap<WString, int> mExtensionToFID;

		WString magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;
		TextureDataPtr importRawImage(DataStreamPtr fileData);
	};
}