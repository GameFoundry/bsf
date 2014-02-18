#include "CmImporter.h"
#include "CmPath.h"
#include "CmResource.h"
#include "CmFileSystem.h"
#include "CmSpecificImporter.h"
#include "CmGpuProgIncludeImporter.h"
#include "CmGpuProgramImporter.h"
#include "CmImportOptions.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmException.h"

namespace CamelotFramework
{
	Importer::Importer()
	{
		registerAssetImporter(cm_new<GpuProgIncludeImporter>());
		registerAssetImporter(cm_new<GpuProgramImporter>());
	}

	Importer::~Importer()
	{
		for(auto i = mAssetImporters.begin(); i != mAssetImporters.end(); ++i)
		{
			if((*i) != nullptr)
				cm_delete(*i);
		}

		mAssetImporters.clear();
	}

	bool Importer::supportsFileType(const WString& extension) const
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

	HResource Importer::import(const WString& inputFilePath, ConstImportOptionsPtr importOptions)
	{
		if(!FileSystem::isFile(inputFilePath))
		{
			LOGWRN("Trying to import asset that doesn't exists. Asset path: " + toString(inputFilePath));
			return HResource();
		}

		SpecificImporter* importer = getImporterForFile(inputFilePath);
		if(importer == nullptr)
			return HResource();

		if(importOptions == nullptr)
			importOptions = importer->getDefaultImportOptions();
		else
		{
			ConstImportOptionsPtr defaultImportOptions = importer->getDefaultImportOptions();
			if(importOptions->getTypeId() != defaultImportOptions->getTypeId())
			{
				CM_EXCEPT(InvalidParametersException, "Provided import options is not of valid type. " \
					"Expected: " + defaultImportOptions->getTypeName() + ". Got: " + importOptions->getTypeName() + ".");
			}
		}

		HResource importedResource = importer->import(inputFilePath, importOptions);

		return importedResource;
	}

	ImportOptionsPtr Importer::createImportOptions(const WString& inputFilePath)
	{
		if(!FileSystem::isFile(inputFilePath))
		{
			LOGWRN("Trying to import asset that doesn't exists. Asset path: " + toString(inputFilePath));
			return nullptr;
		}

		SpecificImporter* importer = getImporterForFile(inputFilePath);
		if(importer == nullptr)
			return nullptr;

		return importer->createImportOptions();
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

	SpecificImporter* Importer::getImporterForFile(const WString& inputFilePath) const
	{
		WString ext = PathUtil::getExtension(inputFilePath);
		ext = ext.substr(1, ext.size() - 1); // Remove the .
		if(!supportsFileType(ext))
		{
			LOGWRN("There is no importer for the provided file type. (" + toString(inputFilePath) + ")");
			return nullptr;
		}

		for(auto iter = mAssetImporters.begin(); iter != mAssetImporters.end(); ++iter)
		{
			if(*iter != nullptr && (*iter)->isExtensionSupported(ext))
			{
				return *iter;
			}
		}

		return nullptr;
	}
}