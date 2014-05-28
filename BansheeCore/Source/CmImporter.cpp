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
#include "CmUUID.h"
#include "CmResources.h"

namespace BansheeEngine
{
	Importer::Importer()
	{
		_registerAssetImporter(cm_new<GpuProgIncludeImporter>());
		_registerAssetImporter(cm_new<GpuProgramImporter>());
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

	HResource Importer::import(const Path& inputFilePath, ConstImportOptionsPtr importOptions)
	{
		if(!FileSystem::isFile(inputFilePath))
		{
			LOGWRN("Trying to import asset that doesn't exists. Asset path: " + inputFilePath.toString());
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

		ResourcePtr importedResource = importer->import(inputFilePath, importOptions);
		return gResources()._createResourceHandle(importedResource);
	}

	void Importer::reimport(HResource& existingResource, const Path& inputFilePath, ConstImportOptionsPtr importOptions)
	{
		if(!FileSystem::isFile(inputFilePath))
		{
			LOGWRN("Trying to import asset that doesn't exists. Asset path: " + inputFilePath.toString());
			return;
		}

		SpecificImporter* importer = getImporterForFile(inputFilePath);
		if(importer == nullptr)
			return;

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

		ResourcePtr importedResource = importer->import(inputFilePath, importOptions);
		existingResource._setHandleData(importedResource, existingResource.getUUID());
	}

	ImportOptionsPtr Importer::createImportOptions(const Path& inputFilePath)
	{
		if(!FileSystem::isFile(inputFilePath))
		{
			LOGWRN("Trying to import asset that doesn't exists. Asset path: " + inputFilePath.toString());
			return nullptr;
		}

		SpecificImporter* importer = getImporterForFile(inputFilePath);
		if(importer == nullptr)
			return nullptr;

		return importer->createImportOptions();
	}

	void Importer::_registerAssetImporter(SpecificImporter* importer)
	{
		if(!importer)
		{
			LOGWRN("Trying to register a null asset importer!");
			return;
		}

		mAssetImporters.push_back(importer);
	}

	SpecificImporter* Importer::getImporterForFile(const Path& inputFilePath) const
	{
		WString ext = inputFilePath.getWExtension();
		ext = ext.substr(1, ext.size() - 1); // Remove the .
		if(!supportsFileType(ext))
		{
			LOGWRN("There is no importer for the provided file type. (" + inputFilePath.toString() + ")");
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