#include "BsSLPrerequisites.h"
#include "BsPath.h"
#include "BsFileSystem.h"
#include "BsDataStream.h"

// DEBUG ONLY
#include "BsDebug.h"
#include "BsMatrix4.h"

extern "C" {
#include "BsMMAlloc.h"
#include "BsASTFX.h"
#include "BsParserFX.h"
#include "BsLexerFX.h"
}

namespace BansheeEngine
{
	void parseFX(ParseState* parseState, const char* source)
	{
		yyscan_t scanner;
		YY_BUFFER_STATE state;

		if (yylex_init_extra(parseState, &scanner)) {
			// couldn't initialize
			return;
		}

		state = yy_scan_string(source, scanner);

		if (yyparse(parseState, scanner)) {
			// error parsing
			return;
		}

		yy_delete_buffer(state, scanner);

		yylex_destroy(scanner);
	}

	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_SL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	void debugPrint(ASTFXNode* node, String indent)
	{
		LOGWRN(indent + "NODE " + toString(node->type));

		for (int i = 0; i < node->options->count; i++)
		{
			OptionDataType odt = OPTION_LOOKUP[(int)node->options->entries[i].type].dataType;
			if (odt == ODT_Complex)
			{
				LOGWRN(indent + toString(i) + ". " + toString(node->options->entries[i].type));
				debugPrint(node->options->entries[i].value.nodePtr, indent + "\t");
				continue;
			}

			String value;
			switch (odt)
			{
			case ODT_Bool:
				value = toString(node->options->entries[i].value.intValue != 0);
				break;
			case ODT_Int:
				value = toString(node->options->entries[i].value.intValue);
				break;
			case ODT_Float:
				value = toString(node->options->entries[i].value.floatValue);
				break;
			case ODT_String:
				value = node->options->entries[i].value.strValue;
				break;
			case ODT_Matrix:
				{
					Matrix4 mat4 = *(Matrix4*)(node->options->entries[i].value.matrixValue);
					value = toString(mat4);
				}
				break;
			}

			LOGWRN(indent + toString(i) + ". " + toString(node->options->entries[i].type) + " = " + value);
		}
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_SL_EXPORT void* loadPlugin()
	{
		// TODO - Register importer

		// DEBUG
		yydebug = 1;
		Path exampleFX = "D:\\testFX.txt";
		DataStreamPtr file = FileSystem::openFile(exampleFX);

		String contents = file->getAsString();

		ParseState* parseState = parseStateCreate();
		parseFX(parseState, contents.c_str());

		debugPrint(parseState->rootNode, "");
		int bp = 0;

		parseStateDelete(parseState);

		return nullptr;
	}
}