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
			if((*i) != nullptr)
				delete *i;
		}

		mAssetImporters.clear();
	}

	bool Importer::supportsFileType(const std::string& extension) const
	{
		for(auto iter = mAssetImporters.begin(); iter != mAssetImporters.end(); ++iter)
		{
			if(*iter != nullptr && (*iter)->isExtensionSupported(extension))
				return true;
		}

		return false;
	}

	bool Importer::supportsFileType(const UINT8* magicNumber, UINT32 magicNumSize) const
	{
		for(auto iter = mAssetImporters.begin(); iter != mAssetImporters.end(); ++iter)
		{
			if(*iter != nullptr && (*iter)->isMagicNumberSupported(magicNumber, magicNumSize))
				return true;
		}

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

		//SpecificImporter* importer = mAssetImporters[ext];

		//ResourcePtr importedResource = importer->import(inputFilePath);

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

		mAssetImporters.push_back(importer);
	}
}