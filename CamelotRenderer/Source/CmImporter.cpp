#include "CmImporter.h"
#include "CmPath.h"
#include "CmSpecificImporter.h"
#include "CmDebug.h"
#include "CmException.h"

namespace CamelotEngine
{
	Importer::~Importer()
	{
		for(auto i = mAssetImporters.begin(); i != mAssetImporters.end(); ++i)
		{
			SpecificImporter* curImporter = i->second;
			if(curImporter != nullptr)
			{
				const std::vector<String>& supportedExtensions = curImporter->extensions();
				for(auto j = supportedExtensions.begin(); j != supportedExtensions.end(); ++j)
				{
					if(mAssetImporters[*j] == curImporter)
						mAssetImporters[*j] = nullptr;
				}

				delete curImporter;
			}
		}

		mAssetImporters.clear();
	}

	bool Importer::supportsFileType(const std::string& extension)
	{
		auto found = mAssetImporters.find(extension);

		if(found != mAssetImporters.end())
			return found->second != nullptr; // Even if we found it, it can still be null

		return false;
	}

	void Importer::import(const String& inputFilePath, const String& outputFilePath, bool keepReferences)
	{
		if(!Path::exists(inputFilePath))
		{
			LOGWRN("Trying to import asset that doesn't exists. Asset path: " + inputFilePath);
			return;
		}

		String ext = Path::getExtension(inputFilePath);
		ext = ext.substr(1, ext.size() - 1); // Remove the .
		if(!supportsFileType(ext))
		{
			LOGWRN("There is no importer for the provided file type. (" + inputFilePath + ")");
			return;
		}

		SpecificImporter* importer = mAssetImporters[ext];

		ResourcePtr importedResource = importer->import(inputFilePath);

		// TODO - Use AssetDatabase for loading the resource
		// TODO - Serialize the resource to output location
	}

	void Importer::registerAssetImporter(SpecificImporter* importer)
	{
		if(!importer)
		{
			LOGWRN("Trying to register a null asset importer!");
			return;
		}

		const std::vector<String>& supportedExtensions = importer->extensions();
		for(auto i = supportedExtensions.begin(); i != supportedExtensions.end(); ++i)
		{
			SpecificImporter* existingImporter = mAssetImporters[*i];
			if(existingImporter != nullptr)
				delete existingImporter;

			mAssetImporters[*i] = importer;
		}
	}
}