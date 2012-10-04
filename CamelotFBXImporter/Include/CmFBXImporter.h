#pragma once

#include "CmFBXPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace CamelotEngine
{
	class CM_FBX_EXPORT FBXImporter : public SpecificImporter
	{
	public:
		FBXImporter();
		virtual ~FBXImporter();

		/**
		 * @brief	Should only be called by the plugin when its being loaded.
		 */
		static void startUp()
		{
			static FBXImporter* importer = nullptr;
			if(importer == nullptr)
			{
				importer = new FBXImporter();
				Importer::instance().registerAssetImporter(importer);
			}
		}

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(DataStreamPtr fileData);
	private:
		vector<String>::type mExtensions;
		std::unordered_map<String, int> mExtensionToFID;

		String magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;
		TextureDataPtr importRawImage(DataStreamPtr fileData);
	};
}