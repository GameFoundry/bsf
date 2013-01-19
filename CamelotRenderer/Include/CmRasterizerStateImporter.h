#pragma once

#include "CmPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

namespace CamelotEngine
{
	class RasterizerStateImporter : public SpecificImporter
	{
	public:
		RasterizerStateImporter();
		virtual ~RasterizerStateImporter();

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual BaseResourceHandle import(const String& filePath);
	private:
		vector<String>::type mExtensions;
		std::unordered_map<String, int> mExtensionToFID;

		String magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const;
	};
}