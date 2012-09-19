#include "CmFreeImgPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace CamelotEngine
{
	class FreeImgImporter : public SpecificImporter
	{
	public:
		FreeImgImporter();
		virtual ~FreeImgImporter();

		/** Inherited from SpecificImporter */
		virtual const std::vector<String>& extensions() const;

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(const String& filePath);

	private:
		vector<String>::type mExtensions;

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