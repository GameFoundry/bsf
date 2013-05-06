#include "BsEngineGUI.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIWindowFrame.h"

#include "CmFont.h"
#include "CmFontImportOptions.h"
#include "CmImporter.h"
#include "CmRTTIType.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String EngineGUI::DefaultFontPath = "C:\\arial.ttf";
	const UINT32 EngineGUI::DefaultFontSize = 12;

	EngineGUI::EngineGUI()
	{
		// TODO - Normally I want to load this from some file
		
		// TODO - Instead of importing font every time, try to save a resource and then just load it?
		HFont font;

		{
			ImportOptionsPtr fontImportOptions = Importer::instance().createImportOptions(DefaultFontPath);
			if(rtti_is_of_type<FontImportOptions>(fontImportOptions))
			{
				FontImportOptions* importOptions = static_cast<FontImportOptions*>(fontImportOptions.get());

				vector<CamelotFramework::UINT32>::type fontSizes;
				fontSizes.push_back(DefaultFontSize);
				importOptions->setFontSizes(fontSizes);
			}

			font = Importer::instance().import(DefaultFontPath, fontImportOptions);
		}

		GUIElementStyle labelStyle;
		labelStyle.font = font;
		labelStyle.fontSize = DefaultFontSize;

		mSkin.setStyle(GUILabel::getGUITypeName(), labelStyle);
	}

}