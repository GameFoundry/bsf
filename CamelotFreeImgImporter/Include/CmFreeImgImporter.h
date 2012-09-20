#include "CmFreeImgPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"
#include "CmPixelUtil.h"

namespace CamelotEngine
{
	class FreeImgImporter : public SpecificImporter
	{
		struct RawImageData;

	public:
		FreeImgImporter();
		virtual ~FreeImgImporter();

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(DataStream* fileData);

	private:
		vector<String>::type mExtensions;
		std::unordered_map<String, int> mExtensionToFID;

		String magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;
		RawImageData importRawImage(DataStream* fileData);

		struct RawImageData
		{
			RawImageData()
				:width(0), height(0), size(0), format(PF_UNKNOWN)
			{ }

			UINT32 width;
			UINT32 height;
			UINT32 size;
			PixelFormat format;
			MemoryDataStreamPtr data;
		};

		class InitOnStart
		{
		public:
			InitOnStart() 
			{ 
				static FreeImgImporter* importer = nullptr;
				if(importer == nullptr)
				{
					importer = new FreeImgImporter();
					Importer::instance().registerAssetImporter(importer);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure importer is registered on program start
	};
}