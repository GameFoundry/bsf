//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSLFXCompiler.h"
#include "BsGpuProgram.h"
#include <regex>
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsSamplerState.h"
#include "BsRenderAPI.h"
#include "BsDebug.h"
#include "BsShaderManager.h"
#include "BsShaderInclude.h"
#include "BsMatrix4.h"
#include "BsBuiltinResources.h"

extern "C" {
#include "BsMMAlloc.h"
#include "BsParserFX.h"
#include "BsLexerFX.h"
}

using namespace std;

namespace BansheeEngine
{
	// Print out the FX AST, only for debug purposes
	void SLFXDebugPrint(ASTFXNode* node, String indent)
	{
		LOGDBG(indent + "NODE " + toString(node->type));

		for (int i = 0; i < node->options->count; i++)
		{
			OptionDataType odt = OPTION_LOOKUP[(int)node->options->entries[i].type].dataType;
			if (odt == ODT_Complex)
			{
				LOGDBG(indent + toString(i) + ". " + toString(node->options->entries[i].type));
				SLFXDebugPrint(node->options->entries[i].value.nodePtr, indent + "\t");
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

			LOGDBG(indent + toString(i) + ". " + toString(node->options->entries[i].type) + " = " + value);
		}
	}

	BSLFXCompileResult BSLFXCompiler::compile(const String& source)
	{
		BSLFXCompileResult output;

		String parsedSource = source;

		ParseState* parseState = parseStateCreate();
		Vector<CodeBlock> codeBlocks = parseCodeBlocks(parsedSource);
		parseFX(parseState, parsedSource.c_str());

		if (parseState->hasError > 0)
		{
			output.errorMessage = parseState->errorMessage;
			output.errorLine = parseState->errorLine;
			output.errorColumn = parseState->errorColumn;

			parseStateDelete(parseState);
		}
		else
		{
			// Only enable for debug purposes
			//SLFXDebugPrint(parseState->rootNode, "");

			output = parseShader("Shader", parseState, codeBlocks);

			StringStream gpuProgError;
			bool hasError = false;
			if (output.shader != nullptr)
			{
				TechniquePtr bestTechnique = output.shader->getBestTechnique();

				if (bestTechnique != nullptr)
				{
					UINT32 numPasses = bestTechnique->getNumPasses();

					for (UINT32 i = 0; i < numPasses; i++)
					{
						PassPtr pass = bestTechnique->getPass(i);

						auto checkCompileStatus = [&](const String& prefix, const GpuProgramPtr& prog)
						{
							if (prog != nullptr)
							{
								prog->blockUntilCoreInitialized();

								if (!prog->isCompiled())
								{
									hasError = true;
									gpuProgError << prefix <<": " << prog->getCompileErrorMessage() << std::endl;
								}
							}
						};

						checkCompileStatus("Vertex program", pass->getVertexProgram());
						checkCompileStatus("Fragment program", pass->getFragmentProgram());
						checkCompileStatus("Geometry program", pass->getGeometryProgram());
						checkCompileStatus("Hull program", pass->getHullProgram());
						checkCompileStatus("Domain program", pass->getDomainProgram());
						checkCompileStatus("Compute program", pass->getComputeProgram());
					}
				}
			}

			if (hasError)
			{
				output.shader = nullptr;
				output.errorMessage = "Failed compiling GPU program(s): " + gpuProgError.str();
				output.errorLine = 0;
				output.errorColumn = 0;
			}
		}

		return output;
	}

	void BSLFXCompiler::parseFX(ParseState* parseState, const char* source)
	{
		yyscan_t scanner;
		YY_BUFFER_STATE state;

		if (yylex_init_extra(parseState, &scanner))
			return;

		state = yy_scan_string(source, scanner);

		if (yyparse(parseState, scanner))
			return;

		yy_delete_buffer(state, scanner);
		yylex_destroy(scanner);
	}

	Vector<BSLFXCompiler::CodeBlock> BSLFXCompiler::parseCodeBlocks(String& source)
	{
		std::regex pattern = std::regex(R"((Vertex|Fragment|Geometry|Hull|Domain|Compute|Common)\s*=\s*\{)");
		std::smatch matches;

		Vector<CodeBlock> codeBlocks;

		UINT32 offset = 0;
		while (std::regex_search(source.cbegin() + offset, source.cend(), matches, pattern))
		{
			UINT32 idx = (UINT32)codeBlocks.size();

			codeBlocks.push_back(CodeBlock());
			CodeBlock& newBlock = codeBlocks.back();

			std::string type = matches[1].str();
			if (type == "Vertex")
				newBlock.type = CodeBlockType::Vertex;
			else if (type == "Fragment")
				newBlock.type = CodeBlockType::Fragment;
			else if (type == "Geometry")
				newBlock.type = CodeBlockType::Geometry;
			else if (type == "Hull")
				newBlock.type = CodeBlockType::Hull;
			else if (type == "Domain")
				newBlock.type = CodeBlockType::Domain;
			else if (type == "Compute")
				newBlock.type = CodeBlockType::Compute;
			else if (type == "Common")
				newBlock.type = CodeBlockType::Common;

			offset += (UINT32)matches.position() + (UINT32)matches.length();

			StringStream newDataStream;
			newDataStream << "Index = " + toString(idx) + ";";

			StringStream codeStream;
			UINT32 ummatchedBrackets = 1;
			for (UINT32 i = offset; i < (UINT32)source.length(); i++)
			{
				if (source[i] == '{')
					ummatchedBrackets++;

				if (source[i] == '}')
					ummatchedBrackets--;

				if (ummatchedBrackets == 0)
					break;

				if (source[i] == '\r' || source[i] == '\n')
					newDataStream << source[i];

				codeStream << source[i];
			}

			newBlock.code = codeStream.str();

			source.erase(source.cbegin() + offset, source.cbegin() + offset + (UINT32)newBlock.code.size());

			String newData = newDataStream.str();
			source.insert(offset, newData);
			offset += (UINT32)newData.size();
		}

		return codeBlocks;
	}

	void BSLFXCompiler::mergeBlocks(ParseState* mergeInto, ASTFXNode* blocksNode)
	{
		if (blocksNode == nullptr || blocksNode->type != NT_Blocks)
			return;

		ASTFXNode* destBlocksNode = nullptr;
		for (int i = 0; i < mergeInto->rootNode->options->count; i++)
		{
			NodeOption* option = &mergeInto->rootNode->options->entries[i];

			if (option->type == OT_Blocks)
			{
				destBlocksNode = option->value.nodePtr;
				break;
			}
		}

		if (destBlocksNode == nullptr)
		{
			destBlocksNode = nodeCreate(mergeInto->memContext, NT_Blocks);
			NodeOption blocksOption; blocksOption.type = OT_Blocks; blocksOption.value.nodePtr = destBlocksNode;
			nodeOptionsAdd(mergeInto->memContext, mergeInto->rootNode->options, &blocksOption);
		}

		for (int i = 0; i < blocksNode->options->count; i++)
		{
			NodeOption* option = &blocksNode->options->entries[i];

			if (option->type == OT_Block)
			{
				ASTFXNode* dstBlock = nodeCreate(mergeInto->memContext, NT_Block);
				NodeOption blockOption; blockOption.type = OT_Block; blockOption.value.nodePtr = dstBlock;
				nodeOptionsAdd(mergeInto->memContext, destBlocksNode->options, &blockOption);

				ASTFXNode* srcBlock = option->value.nodePtr;
				for (int j = 0; j < srcBlock->options->count; j++)
					copyNodeOption(mergeInto, dstBlock->options, &srcBlock->options->entries[j]);
			}
		}
	}

	void BSLFXCompiler::mergeParameters(ParseState* mergeInto, ASTFXNode* paramsNode)
	{
		if (paramsNode == nullptr || paramsNode->type != NT_Parameters)
			return;

		ASTFXNode* destParamsNode = nullptr;
		for (int i = 0; i < mergeInto->rootNode->options->count; i++)
		{
			NodeOption* option = &mergeInto->rootNode->options->entries[i];

			if (option->type == OT_Parameters)
			{
				destParamsNode = option->value.nodePtr;
				break;
			}
		}

		if (destParamsNode == nullptr)
		{
			destParamsNode = nodeCreate(mergeInto->memContext, NT_Parameters);
			NodeOption paramsOption; paramsOption.type = OT_Parameters; paramsOption.value.nodePtr = destParamsNode;
			nodeOptionsAdd(mergeInto->memContext, mergeInto->rootNode->options, &paramsOption);
		}

		for (int i = 0; i < paramsNode->options->count; i++)
		{
			NodeOption* option = &paramsNode->options->entries[i];

			if (option->type == OT_Parameter)
			{
				ASTFXNode* dstParam = nodeCreate(mergeInto->memContext, NT_Parameter);
				NodeOption paramOption; paramOption.type = OT_Parameter; paramOption.value.nodePtr = dstParam;
				nodeOptionsAdd(mergeInto->memContext, destParamsNode->options, &paramOption);

				ASTFXNode* srcParam = option->value.nodePtr;
				for (int j = 0; j < srcParam->options->count; j++)
					copyNodeOption(mergeInto, dstParam->options, &srcParam->options->entries[j]);
			}
		}
	}

	void BSLFXCompiler::getTechniqueIdentifier(ASTFXNode* technique, StringID& renderer, String& language)
	{
		renderer = RendererAny;
		language = "Any";

		for (int i = 0; i < technique->options->count; i++)
		{
			NodeOption* option = &technique->options->entries[i];

			switch (option->type)
			{
			case OT_Renderer:
				renderer = parseRenderer(removeQuotes(option->value.strValue));
				break;
			case OT_Language:
				language = removeQuotes(option->value.strValue);
				break;
			}
		}
	}

	bool BSLFXCompiler::isTechniqueMergeValid(ASTFXNode* into, ASTFXNode* from)
	{
		StringID intoRenderer = RendererAny;
		String intoLanguage = "Any";

		StringID fromRenderer = RendererAny;
		String fromLanguage = "Any";

		getTechniqueIdentifier(into, intoRenderer, intoLanguage);
		getTechniqueIdentifier(from, fromRenderer, fromLanguage);

		return (intoRenderer == fromRenderer || fromRenderer == RendererAny) && (intoLanguage == fromLanguage || fromLanguage == "Any");
	}

	bool BSLFXCompiler::copyNodeOption(ParseState* into, NodeOptions* parent, NodeOption* option)
	{
		OptionDataType dataType = OPTION_LOOKUP[(int)option->type].dataType;
		if (dataType == ODT_Complex)
		{
			ASTFXNode* newNode = nullptr;

			if (option->value.nodePtr != nullptr)
				newNode = nodeCreate(into->memContext, option->value.nodePtr->type);

			NodeOption newOption;
			newOption.type = option->type;
			newOption.value.nodePtr = newNode;

			nodeOptionsAdd(into->memContext, parent, &newOption);

			return true;
		}
		else if (dataType == ODT_String)
		{
			NodeOption newOption;
			newOption.type = option->type;
			newOption.value.strValue = mmalloc_strdup(into->memContext, option->value.strValue);

			nodeOptionsAdd(into->memContext, parent, &newOption);
			return false;
		}
		else
		{
			nodeOptionsAdd(into->memContext, parent, option);
			return false;
		}
	}

	void BSLFXCompiler::mergePass(ParseState* mergeInto, ASTFXNode* mergeIntoNode, ASTFXNode* mergeFromNode, UINT32 codeBlockOffset)
	{
		if (mergeIntoNode == nullptr || mergeIntoNode->type != NT_Pass || mergeFromNode == nullptr || mergeFromNode->type != NT_Pass)
			return;

		mergeCode(mergeInto, mergeIntoNode, mergeFromNode, codeBlockOffset);
		mergePassStates(mergeInto, mergeIntoNode, mergeFromNode);
	}

	void BSLFXCompiler::mergeCode(ParseState* mergeInto, ASTFXNode* mergeIntoNode, ASTFXNode* mergeFromNode, UINT32 codeBlockOffset)
	{
		for (int i = 0; i < mergeFromNode->options->count; i++)
		{
			NodeOption* option = &mergeFromNode->options->entries[i];

			if (option->type == OT_Code)
			{
				ASTFXNode* srcCodeNode = option->value.nodePtr;

				UINT32 origIndex = 0;
				for (int j = 0; j < srcCodeNode->options->count; j++)
				{
					NodeOption* codeOption = &srcCodeNode->options->entries[j];
					if (codeOption->type == OT_Index)
						origIndex = codeOption->value.intValue;
				}

				ASTFXNode* destCodeNode = nodeCreate(mergeInto->memContext, NT_Code);
				NodeOption index; index.type = OT_Index; index.value.intValue = codeBlockOffset + origIndex;
				nodeOptionsAdd(mergeInto->memContext, destCodeNode->options, &index);

				NodeOption code; code.type = OT_Code; code.value.nodePtr = destCodeNode;
				nodeOptionsAdd(mergeInto->memContext, mergeIntoNode->options, &code);
			}
		}
	}

	void BSLFXCompiler::mergePassStates(ParseState* mergeInto, ASTFXNode* mergeIntoNode, ASTFXNode* mergeFromNode)
	{
		for (int i = 0; i < mergeFromNode->options->count; i++)
		{
			NodeOption* option = &mergeFromNode->options->entries[i];

			// Skip these as we handle them specially elsewhere
			if (option->type == OT_Code || option->type == OT_Pass || option->type == OT_Renderer || option->type == OT_Language)
				continue;

			if (copyNodeOption(mergeInto, mergeIntoNode->options, option))
			{
				ASTFXNode* newNode = mergeIntoNode->options->entries[mergeIntoNode->options->count - 1].value.nodePtr;
				mergePassStates(mergeInto, newNode, option->value.nodePtr);
			}
		}
	}

	void BSLFXCompiler::mergeTechnique(ParseState* mergeInto, ASTFXNode* mergeIntoNode, ASTFXNode* mergeFromNode, UINT32 codeBlockOffset)
	{
		if (mergeIntoNode == nullptr || mergeIntoNode->type != NT_Technique || mergeFromNode == nullptr || mergeFromNode->type != NT_Technique)
			return;

		mergeCode(mergeInto, mergeIntoNode, mergeFromNode, codeBlockOffset);
		mergePassStates(mergeInto, mergeIntoNode, mergeFromNode);

		Map<UINT32, ASTFXNode*, std::greater<UINT32>> fromPasses;

		UINT32 nextPassIdx = 0;
		for (int i = 0; i < mergeFromNode->options->count; i++)
		{
			NodeOption* option = &mergeFromNode->options->entries[i];

			if (option->type == OT_Pass)
			{
				ASTFXNode* passNode = option->value.nodePtr;
				UINT32 passIdx = nextPassIdx;
				for (int j = 0; j < passNode->options->count; j++)
				{
					NodeOption* passOption = &passNode->options->entries[i];

					if (passOption->type == OT_Index)
						passIdx = (UINT32)passOption->value.intValue;
				}

				fromPasses[passIdx] = passNode;
				nextPassIdx = std::max(nextPassIdx, passIdx) + 1;
			}
		}

		Map<UINT32, ASTFXNode*, std::greater<UINT32>> intoPasses;

		nextPassIdx = 0;
		for (int i = 0; i < mergeIntoNode->options->count; i++)
		{
			NodeOption* option = &mergeIntoNode->options->entries[i];

			if (option->type == OT_Pass)
			{
				ASTFXNode* passNode = option->value.nodePtr;
				UINT32 passIdx = nextPassIdx;
				for (int j = 0; j < passNode->options->count; j++)
				{
					NodeOption* passOption = &passNode->options->entries[i];

					if (passOption->type == OT_Index)
						passIdx = (UINT32)passOption->value.intValue;
				}

				intoPasses[passIdx] = passNode;
				nextPassIdx = std::max(nextPassIdx, passIdx) + 1;
			}
		}

		for (auto& fromPair : fromPasses)
		{
			auto iterFind = intoPasses.find(fromPair.first);
			if (iterFind != intoPasses.end())
			{
				mergePass(mergeInto, iterFind->second, fromPair.second, codeBlockOffset);
			}
			else
			{
				ASTFXNode* passNode = nodeCreate(mergeInto->memContext, NT_Pass);
				NodeOption passOption; passOption.type = OT_Pass; passOption.value.nodePtr = passNode;
				nodeOptionsAdd(mergeInto->memContext, mergeIntoNode->options, &passOption);

				mergePass(mergeInto, passNode, fromPair.second, codeBlockOffset);
			}
		}
	}

	void BSLFXCompiler::mergeTechniques(ParseState* mergeInto, ASTFXNode* techniqueNode, UINT32 codeBlockOffset)
	{
		if (techniqueNode == nullptr || techniqueNode->type != NT_Technique)
			return;

		bool anyMerged = false;
		for (int i = 0; i < mergeInto->rootNode->options->count; i++)
		{
			NodeOption* option = &mergeInto->rootNode->options->entries[i];

			if (option->type == OT_Technique)
			{
				if (isTechniqueMergeValid(option->value.nodePtr, techniqueNode))
				{
					mergeTechnique(mergeInto, option->value.nodePtr, techniqueNode, codeBlockOffset);
					anyMerged = true;
					break;
				}
			}
		}

		// If this is a language specific technique and it wasn't merged with anything, create a new technique node
		if (!anyMerged)
		{
			ASTFXNode* techniqueNode = nodeCreate(mergeInto->memContext, NT_Technique);
			NodeOption techniqueOption; techniqueOption.type = OT_Technique; techniqueOption.value.nodePtr = techniqueNode;
			nodeOptionsAdd(mergeInto->memContext, mergeInto->rootNode->options, &techniqueOption);

			mergeTechnique(mergeInto, techniqueNode, techniqueNode, codeBlockOffset);
		}
	}

	void BSLFXCompiler::mergeAST(ParseState* mergeInto, ASTFXNode* mergeFrom, UINT32 codeBlockOffset)
	{
		if (mergeInto->rootNode->type != NT_Shader || mergeFrom == nullptr || mergeFrom->type != NT_Shader)
			return;

		for (int i = 0; i < mergeFrom->options->count; i++)
		{
			NodeOption* option = &mergeFrom->options->entries[i];

			switch (option->type)
			{
			case OT_Separable:
			case OT_Queue:
			case OT_Priority:
			case OT_Transparent:
				nodeOptionsAdd(mergeInto->memContext, mergeInto->rootNode->options, option);
				break;
			case OT_Technique:
				mergeTechniques(mergeInto, option->value.nodePtr, codeBlockOffset);
				break;
			case OT_Parameters:
				mergeParameters(mergeInto, option->value.nodePtr);
				break;
			case OT_Blocks:
				mergeBlocks(mergeInto, option->value.nodePtr);
				break;
			}
		}
	}

	StringID BSLFXCompiler::parseRenderer(const String& name)
	{
		if (name == "Any")
			return RendererAny;
		else if (name == "Default")
			return RendererDefault;

		return RendererAny;
	}

	void BSLFXCompiler::parseLanguage(const String& name, StringID& renderAPI, String& language)
	{
		if (name == "HLSL" || name == "HLSL11")
		{
			renderAPI = RenderAPIDX11;
			language = "hlsl";
		}
		else if (name == "HLSL9")
		{
			renderAPI = RenderAPIDX9;
			language = "hlsl9";
		}
		else if (name == "GLSL")
		{
			renderAPI = RenderAPIOpenGL;
			language = "glsl";
		}
		else // "Any"
		{
			renderAPI = RenderAPIAny;
			language = "";
		}
	}

	GpuParamBlockUsage BSLFXCompiler::parseBlockUsage(BufferUsageValue usage)
	{
		if (usage == BUV_Dynamic)
			return GPBU_DYNAMIC;

		return GPBU_STATIC;
	}

	UINT32 BSLFXCompiler::parseFilterMode(FilterValue filter)
	{
		switch (filter)
		{
		case FV_Point:
			return FO_POINT;
		case FV_Linear:
			return FO_LINEAR;
		case FV_Anisotropic:
			return FO_ANISOTROPIC;
		case FV_PointCmp:
			return FO_POINT | FO_USE_COMPARISON;
		case FV_LinearCmp:
			return FO_LINEAR | FO_USE_COMPARISON;
		case FV_AnisotropicCmp:
			return FO_ANISOTROPIC | FO_USE_COMPARISON;
		}

		return FO_NONE;
	}

	CompareFunction BSLFXCompiler::parseCompFunc(CompFuncValue compFunc)
	{
		switch (compFunc)
		{
		case CFV_Pass:
			return CMPF_ALWAYS_PASS;
		case CFV_Fail:
			return CMPF_ALWAYS_FAIL;
		case CFV_LT:
			return CMPF_LESS;
		case CFV_LTE:
			return CMPF_LESS_EQUAL;
		case CFV_EQ:
			return CMPF_EQUAL;
		case CFV_NEQ:
			return CMPF_NOT_EQUAL;
		case CFV_GT:
			return CMPF_GREATER;
		case CFV_GTE:
			return CMPF_GREATER_EQUAL;
		}

		return CMPF_ALWAYS_PASS;
	}

	TextureAddressingMode BSLFXCompiler::parseAddrMode(AddrModeValue addrMode)
	{
		switch (addrMode)
		{
		case AMV_Wrap:
			return TAM_WRAP;
		case AMV_Mirror:
			return TAM_MIRROR;
		case AMV_Clamp:
			return TAM_CLAMP;
		case AMV_Border:
			return TAM_BORDER;
		}

		return TAM_WRAP;
	}

	BlendFactor BSLFXCompiler::parseBlendFactor(OpValue factor)
	{
		switch (factor)
		{
		case OV_One:
			return BF_ONE;
		case OV_Zero:
			return BF_ZERO;
		case OV_DestColor:
			return BF_DEST_COLOR;
		case OV_SrcColor:
			return BF_SOURCE_COLOR;
		case OV_InvDestColor:
			return BF_INV_DEST_COLOR;
		case OV_InvSrcColor:
			return BF_INV_SOURCE_COLOR;
		case OV_DestAlpha:
			return BF_DEST_ALPHA;
		case OV_SrcAlpha:
			return BF_SOURCE_ALPHA;
		case OV_InvDestAlpha:
			return BF_INV_DEST_ALPHA;
		case OV_InvSrcAlpha:
			return BF_INV_SOURCE_ALPHA;
		}

		return BF_ONE;
	}

	BlendOperation BSLFXCompiler::parseBlendOp(BlendOpValue op)
	{
		switch (op)
		{
		case BOV_Add:
			return BO_ADD;
		case BOV_Max:
			return BO_MAX;
		case BOV_Min:
			return BO_MIN;
		case BOV_Subtract:
			return BO_SUBTRACT;
		case BOV_RevSubtract:
			return BO_REVERSE_SUBTRACT;
		}

		return BO_ADD;
	}

	void BSLFXCompiler::parseParamType(ParamType type, bool& isObjType, UINT32& typeId)
	{
		struct ParamData
		{
			UINT32 type;
			bool isObjType;
		};

		static bool initialized = false;
		static ParamData lookup[PT_Count];

		if (!initialized)
		{
			lookup[PT_Float] = { { GPDT_FLOAT1 }, false };
			lookup[PT_Float2] = { { GPDT_FLOAT2 }, false };
			lookup[PT_Float3] = { { GPDT_FLOAT3 }, false };
			lookup[PT_Float4] = { { GPDT_FLOAT4 }, false };

			lookup[PT_Int] = { { GPDT_INT1 }, false };
			lookup[PT_Int2] = { { GPDT_INT2 }, false };
			lookup[PT_Int3] = { { GPDT_INT3 }, false };
			lookup[PT_Int4] = { { GPDT_INT4 }, false };

			lookup[PT_Mat2x2] = { { GPDT_MATRIX_2X2 }, false };
			lookup[PT_Mat2x3] = { { GPDT_MATRIX_2X3 }, false };
			lookup[PT_Mat2x4] = { { GPDT_MATRIX_2X4 }, false };

			lookup[PT_Mat3x2] = { { GPDT_MATRIX_3X2 }, false };
			lookup[PT_Mat3x3] = { { GPDT_MATRIX_3X3 }, false };
			lookup[PT_Mat3x4] = { { GPDT_MATRIX_3X4 }, false };

			lookup[PT_Mat4x2] = { { GPDT_MATRIX_4X2 }, false };
			lookup[PT_Mat4x3] = { { GPDT_MATRIX_4X3 }, false };
			lookup[PT_Mat4x4] = { { GPDT_MATRIX_4X4 }, false };

			lookup[PT_Sampler1D] = { { GPOT_SAMPLER1D }, true };
			lookup[PT_Sampler2D] = { { GPOT_SAMPLER2D }, true };
			lookup[PT_Sampler3D] = { { GPOT_SAMPLER3D }, true };
			lookup[PT_SamplerCUBE] = { { GPOT_SAMPLERCUBE }, true };
			lookup[PT_Sampler2DMS] = { { GPOT_SAMPLER2DMS }, true };

			lookup[PT_Texture1D] = { { GPOT_TEXTURE1D }, true };
			lookup[PT_Texture2D] = { { GPOT_TEXTURE2D }, true };
			lookup[PT_Texture3D] = { { GPOT_TEXTURE3D }, true };
			lookup[PT_TextureCUBE] = { { GPOT_TEXTURECUBE }, true };
			lookup[PT_Texture2DMS] = { { GPOT_TEXTURE2DMS }, true };

			lookup[PT_ByteBuffer] = { { GPOT_BYTE_BUFFER }, true };
			lookup[PT_StructBuffer] = { { GPOT_STRUCTURED_BUFFER }, true };
			lookup[PT_TypedBufferRW] = { { GPOT_RWTYPED_BUFFER }, true };
			lookup[PT_ByteBufferRW] = { { GPOT_RWBYTE_BUFFER }, true };
			lookup[PT_StructBufferRW] = { { GPOT_RWSTRUCTURED_BUFFER }, true };
			lookup[PT_AppendBuffer] = { { GPOT_RWAPPEND_BUFFER }, true };
			lookup[PT_ConsumeBuffer] = { { GPOT_RWCONSUME_BUFFER }, true };

			initialized = true;
		}

		isObjType = lookup[type].isObjType;
		typeId = lookup[type].type;
	}

	StencilOperation BSLFXCompiler::parseStencilOp(OpValue op)
	{
		switch (op)
		{
		case OV_Keep:
			return SOP_KEEP;
		case OV_Zero:
			return SOP_ZERO;
		case OV_Replace:
			return SOP_REPLACE;
		case OV_Incr:
			return SOP_INCREMENT;
		case OV_Decr:
			return SOP_DECREMENT;
		case OV_IncrWrap:
			return SOP_INCREMENT_WRAP;
		case OV_DecrWrap:
			return SOP_DECREMENT_WRAP;
		case OV_Invert:
			return SOP_INVERT;
		}

		return SOP_KEEP;
	}

	CullingMode BSLFXCompiler::parseCullMode(CullModeValue cm)
	{
		switch (cm)
		{
		case CMV_None:
			return CULL_NONE;
		case CMV_CW:
			return CULL_CLOCKWISE;
		case CMV_CCW:
			return CULL_COUNTERCLOCKWISE;
		}

		return CULL_COUNTERCLOCKWISE;
	}

	PolygonMode BSLFXCompiler::parseFillMode(FillModeValue fm)
	{
		if (fm == FMV_Wire)
			return PM_WIREFRAME;

		return PM_SOLID;
	}

	void BSLFXCompiler::parseStencilFront(DEPTH_STENCIL_STATE_DESC& desc, ASTFXNode* stencilOpNode)
	{
		if (stencilOpNode == nullptr || stencilOpNode->type != NT_StencilOp)
			return;

		for (int i = 0; i < stencilOpNode->options->count; i++)
		{
			NodeOption* option = &stencilOpNode->options->entries[i];

			switch (option->type)
			{
			case OT_Fail:
				desc.frontStencilFailOp = parseStencilOp((OpValue)option->value.intValue);
				break;
			case OT_ZFail:
				desc.frontStencilZFailOp = parseStencilOp((OpValue)option->value.intValue);
				break;
			case OT_PassOp:
				desc.frontStencilPassOp = parseStencilOp((OpValue)option->value.intValue);
				break;
			case OT_CompareFunc:
				desc.frontStencilComparisonFunc = parseCompFunc((CompFuncValue)option->value.intValue);
				break;
			}
		}
	}

	void BSLFXCompiler::parseStencilBack(DEPTH_STENCIL_STATE_DESC& desc, ASTFXNode* stencilOpNode)
	{
		if (stencilOpNode == nullptr || stencilOpNode->type != NT_StencilOp)
			return;

		for (int i = 0; i < stencilOpNode->options->count; i++)
		{
			NodeOption* option = &stencilOpNode->options->entries[i];

			switch (option->type)
			{
			case OT_Fail:
				desc.backStencilFailOp = parseStencilOp((OpValue)option->value.intValue);
				break;
			case OT_ZFail:
				desc.backStencilZFailOp = parseStencilOp((OpValue)option->value.intValue);
				break;
			case OT_PassOp:
				desc.backStencilPassOp = parseStencilOp((OpValue)option->value.intValue);
				break;
			case OT_CompareFunc:
				desc.backStencilComparisonFunc = parseCompFunc((CompFuncValue)option->value.intValue);
				break;
			}
		}
	}

	void BSLFXCompiler::parseAddrMode(SAMPLER_STATE_DESC& desc, ASTFXNode* addrModeNode)
	{
		if (addrModeNode == nullptr || addrModeNode->type != NT_AddrMode)
			return;

		for (int i = 0; i < addrModeNode->options->count; i++)
		{
			NodeOption* option = &addrModeNode->options->entries[i];

			switch (option->type)
			{
			case OT_U:
				desc.addressMode.u = parseAddrMode((AddrModeValue)option->value.intValue);
				break;
			case OT_V:
				desc.addressMode.v = parseAddrMode((AddrModeValue)option->value.intValue);
				break;
			case OT_W:
				desc.addressMode.w = parseAddrMode((AddrModeValue)option->value.intValue);
				break;
			}
		}
	}

	void BSLFXCompiler::parseColorBlendDef(RENDER_TARGET_BLEND_STATE_DESC& desc, ASTFXNode* blendDefNode)
	{
		if (blendDefNode == nullptr || blendDefNode->type != NT_BlendDef)
			return;

		for (int i = 0; i < blendDefNode->options->count; i++)
		{
			NodeOption* option = &blendDefNode->options->entries[i];

			switch (option->type)
			{
			case OT_Source:
				desc.srcBlend = parseBlendFactor((OpValue)option->value.intValue);
				break;
			case OT_Dest:
				desc.dstBlend = parseBlendFactor((OpValue)option->value.intValue);
				break;
			case OT_Op:
				desc.blendOp = parseBlendOp((BlendOpValue)option->value.intValue);
				break;
			}
		}
	}

	void BSLFXCompiler::parseAlphaBlendDef(RENDER_TARGET_BLEND_STATE_DESC& desc, ASTFXNode* blendDefNode)
	{
		if (blendDefNode == nullptr || blendDefNode->type != NT_BlendDef)
			return;

		for (int i = 0; i < blendDefNode->options->count; i++)
		{
			NodeOption* option = &blendDefNode->options->entries[i];

			switch (option->type)
			{
			case OT_Source:
				desc.srcBlendAlpha = parseBlendFactor((OpValue)option->value.intValue);
				break;
			case OT_Dest:
				desc.dstBlendAlpha = parseBlendFactor((OpValue)option->value.intValue);
				break;
			case OT_Op:
				desc.blendOpAlpha = parseBlendOp((BlendOpValue)option->value.intValue);
				break;
			}
		}
	}

	void BSLFXCompiler::parseRenderTargetBlendState(BLEND_STATE_DESC& desc, ASTFXNode* targetNode)
	{
		if (targetNode == nullptr || targetNode->type != NT_Target)
			return;

		UINT32 index = 0;

		for (int i = 0; i < targetNode->options->count; i++)
		{
			NodeOption* option = &targetNode->options->entries[i];

			switch (option->type)
			{
			case OT_Index:
				index = option->value.intValue;
				break;
			}
		}

		if (index >= BS_MAX_MULTIPLE_RENDER_TARGETS)
			return;

		RENDER_TARGET_BLEND_STATE_DESC& rtDesc = desc.renderTargetDesc[index];
		for (int i = 0; i < targetNode->options->count; i++)
		{
			NodeOption* option = &targetNode->options->entries[i];

			switch (option->type)
			{
			case OT_Blend:
				rtDesc.blendEnable = option->value.intValue > 0;
				break;
			case OT_Color:
				parseColorBlendDef(rtDesc, option->value.nodePtr);
				break;
			case OT_Alpha:
				parseAlphaBlendDef(rtDesc, option->value.nodePtr);
				break;
			case OT_WriteMask:
				rtDesc.renderTargetWriteMask = option->value.intValue;
				break;
			}
		}
	}

	bool BSLFXCompiler::parseBlendState(BLEND_STATE_DESC& desc, ASTFXNode* passNode)
	{
		if (passNode == nullptr || (passNode->type != NT_Pass && passNode->type != NT_Technique))
			return false;

		bool default = true;

		for (int i = 0; i < passNode->options->count; i++)
		{
			NodeOption* option = &passNode->options->entries[i];

			switch (option->type)
			{
			case OT_AlphaToCoverage:
				desc.alphaToCoverageEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_IndependantBlend:
				desc.independantBlendEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_Target:
				parseRenderTargetBlendState(desc, option->value.nodePtr);
				default = false;
				break;
			}
		}

		return !default;
	}

	bool BSLFXCompiler::parseRasterizerState(RASTERIZER_STATE_DESC& desc, ASTFXNode* passNode)
	{
		if (passNode == nullptr || (passNode->type != NT_Pass && passNode->type != NT_Technique))
			return false;

		bool default = true;

		for (int i = 0; i < passNode->options->count; i++)
		{
			NodeOption* option = &passNode->options->entries[i];

			switch (option->type)
			{
			case OT_FillMode:
				desc.polygonMode = parseFillMode((FillModeValue)option->value.intValue);
				default = false;
				break;
			case OT_CullMode:
				desc.cullMode = parseCullMode((CullModeValue)option->value.intValue);
				default = false;
				break;
			case OT_DepthBias:
				desc.depthBias = option->value.floatValue;
				default = false;
				break;
			case OT_SDepthBias:
				desc.slopeScaledDepthBias = option->value.floatValue;
				default = false;
				break;
			case OT_DepthClip:
				desc.depthClipEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_Scissor:
				desc.scissorEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_Multisample:
				desc.multisampleEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_AALine:
				desc.antialiasedLineEnable = option->value.intValue > 0;
				default = false;
				break;
			}
		}

		return !default;
	}

	bool BSLFXCompiler::parseDepthStencilState(DEPTH_STENCIL_STATE_DESC& desc, ASTFXNode* passNode)
	{
		if (passNode == nullptr || (passNode->type != NT_Pass && passNode->type != NT_Technique))
			return false;

		bool default = true;

		for (int i = 0; i < passNode->options->count; i++)
		{
			NodeOption* option = &passNode->options->entries[i];

			switch (option->type)
			{
			case OT_DepthRead:
				desc.depthReadEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_DepthWrite:
				desc.depthWriteEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_CompareFunc:
				desc.depthComparisonFunc = parseCompFunc((CompFuncValue)option->value.intValue);
				default = false;
				break;
			case OT_Stencil:
				desc.stencilEnable = option->value.intValue > 0;
				default = false;
				break;
			case OT_StencilReadMask:
				desc.stencilReadMask = (UINT8)option->value.intValue;
				default = false;
				break;
			case OT_StencilWriteMask:
				desc.stencilWriteMask = (UINT8)option->value.intValue;
				default = false;
				break;
			case OT_StencilOpFront:
				parseStencilFront(desc, option->value.nodePtr);
				default = false;
				break;
			case OT_StencilOpBack:
				parseStencilBack(desc, option->value.nodePtr);
				default = false;
				break;
			}
		}

		return !default;
	}

	SamplerStatePtr BSLFXCompiler::parseSamplerState(ASTFXNode* samplerStateNode)
	{
		if (samplerStateNode == nullptr || samplerStateNode->type != NT_SamplerState)
			return nullptr;

		SAMPLER_STATE_DESC desc;
		bool default = true;

		for (int i = 0; i < samplerStateNode->options->count; i++)
		{
			NodeOption* option = &samplerStateNode->options->entries[i];

			switch (option->type)
			{
			case OT_AddrMode:
				parseAddrMode(desc, option->value.nodePtr);
				default = false;
				break;
			case OT_MinFilter:
				desc.minFilter = (FilterOptions)parseFilterMode((FilterValue)option->value.intValue);
				default = false;
				break;
			case OT_MagFilter:
				desc.magFilter = (FilterOptions)parseFilterMode((FilterValue)option->value.intValue);
				default = false;
				break;
			case OT_MipFilter:
				desc.mipFilter = (FilterOptions)parseFilterMode((FilterValue)option->value.intValue);
				default = false;
				break;
			case OT_MaxAniso:
				desc.maxAniso = option->value.intValue;
				default = false;
				break;
			case OT_MipBias:
				desc.mipmapBias = option->value.floatValue;
				default = false;
				break;
			case OT_MipMin:
				desc.mipMin = option->value.floatValue;
				default = false;
				break;
			case OT_MipMax:
				desc.mipMax = option->value.floatValue;
				default = false;
				break;
			case OT_BorderColor:
				desc.borderColor = Color(option->value.matrixValue[0], option->value.matrixValue[1],
					option->value.matrixValue[2], option->value.matrixValue[3]);
				default = false;
				break;
			case OT_CompareFunc:
				desc.comparisonFunc = parseCompFunc((CompFuncValue)option->value.intValue);
				default = false;
				break;
			}
		}

		if (default)
			return nullptr;

		return SamplerState::create(desc);
	}

	void BSLFXCompiler::parseCodeBlock(ASTFXNode* codeNode, const Vector<CodeBlock>& codeBlocks, PassData& passData)
	{
		if (codeNode == nullptr || codeNode->type != NT_Code)
			return;

		UINT32 index = (UINT32)-1;
		for (int j = 0; j < codeNode->options->count; j++)
		{
			if (codeNode->options->entries[j].type == OT_Index)
				index = codeNode->options->entries[j].value.intValue;
		}

		if (index != (UINT32)-1 && index < (UINT32)codeBlocks.size())
		{
			const CodeBlock& codeBlock = codeBlocks[index];
			switch (codeBlock.type)
			{
			case CodeBlockType::Vertex:
				passData.vertexCode += codeBlock.code;
				break;
			case CodeBlockType::Fragment:
				passData.fragmentCode += codeBlock.code;
				break;
			case CodeBlockType::Geometry:
				passData.geometryCode += codeBlock.code;
				break;
			case CodeBlockType::Hull:
				passData.hullCode += codeBlock.code;
				break;
			case CodeBlockType::Domain:
				passData.domainCode += codeBlock.code;
				break;
			case CodeBlockType::Compute:
				passData.computeCode += codeBlock.code;
				break;
			case CodeBlockType::Common:
				passData.commonCode += codeBlock.code;
				break;
			}
		}
	}

	PassPtr BSLFXCompiler::parsePass(ASTFXNode* passNode, const Vector<CodeBlock>& codeBlocks, PassData& passData, 
		const StringID& renderAPI, const String& language, UINT32& seqIdx)
	{
		if (passNode == nullptr || passNode->type != NT_Pass)
			return nullptr;

		PASS_DESC passDesc;

		passData.blendIsDefault &= !parseBlendState(passData.blendDesc, passNode);
		passData.rasterizerIsDefault &= !parseRasterizerState(passData.rasterizerDesc, passNode);
		passData.depthStencilIsDefault &= !parseDepthStencilState(passData.depthStencilDesc, passNode);

		if (!passData.blendIsDefault)
			passDesc.blendState = BlendState::create(passData.blendDesc);

		if (!passData.rasterizerIsDefault)
			passDesc.rasterizerState = RasterizerState::create(passData.rasterizerDesc);

		if (!passData.depthStencilIsDefault)
			passDesc.depthStencilState = DepthStencilState::create(passData.depthStencilDesc);

		for (int i = 0; i < passNode->options->count; i++)
		{
			NodeOption* option = &passNode->options->entries[i];

			switch (option->type)
			{
			case OT_Index:
				seqIdx = option->value.intValue;
				break;
			case OT_StencilRef:
				passDesc.stencilRefValue = option->value.intValue;
				break;
			case OT_Code:
				parseCodeBlock(option->value.nodePtr, codeBlocks, passData);
				break;
			}
		}

		if (!passData.vertexCode.empty())
		{
			passDesc.vertexProgram = GpuProgram::create(passData.commonCode + passData.vertexCode, "main", language,
				GPT_VERTEX_PROGRAM, getProfile(renderAPI, GPT_VERTEX_PROGRAM));
		}

		if (!passData.fragmentCode.empty())
		{
			passDesc.fragmentProgram = GpuProgram::create(passData.commonCode + passData.fragmentCode, "main", language,
				GPT_FRAGMENT_PROGRAM, getProfile(renderAPI, GPT_FRAGMENT_PROGRAM));
		}

		if (!passData.geometryCode.empty())
		{
			passDesc.geometryProgram = GpuProgram::create(passData.commonCode + passData.geometryCode, "main", language,
				GPT_GEOMETRY_PROGRAM, getProfile(renderAPI, GPT_GEOMETRY_PROGRAM));
		}

		if (!passData.hullCode.empty())
		{
			passDesc.hullProgram = GpuProgram::create(passData.commonCode + passData.hullCode, "main", language,
				GPT_HULL_PROGRAM, getProfile(renderAPI, GPT_HULL_PROGRAM));
		}

		if (!passData.domainCode.empty())
		{
			passDesc.domainProgram = GpuProgram::create(passData.commonCode + passData.domainCode, "main", language,
				GPT_DOMAIN_PROGRAM, getProfile(renderAPI, GPT_DOMAIN_PROGRAM));
		}

		if (!passData.computeCode.empty())
		{
			passDesc.computeProgram = GpuProgram::create(passData.commonCode + passData.computeCode, "main", language,
				GPT_COMPUTE_PROGRAM, getProfile(renderAPI, GPT_COMPUTE_PROGRAM));
		}

		return Pass::create(passDesc);
	}

	TechniquePtr BSLFXCompiler::parseTechnique(ASTFXNode* techniqueNode, const Vector<CodeBlock>& codeBlocks)
	{
		if (techniqueNode == nullptr || techniqueNode->type != NT_Technique)
			return nullptr;

		Vector<ASTFXNode*> passNodes;
		StringID renderer = RendererAny;
		StringID renderAPI = RenderAPIAny;
		String language;
		
		PassData commonPassData;
		for (int i = 0; i < techniqueNode->options->count; i++)
		{
			NodeOption* option = &techniqueNode->options->entries[i];

			switch (option->type)
			{
			case OT_Pass:
				passNodes.push_back(option->value.nodePtr);
				break;
			case OT_Renderer:
				renderer = parseRenderer(removeQuotes(option->value.strValue));
				break;
			case OT_Language:
				parseLanguage(removeQuotes(option->value.strValue), renderAPI, language);
				break;
			case OT_Code:
				parseCodeBlock(option->value.nodePtr, codeBlocks, commonPassData);
				break;
			}
		}

		commonPassData.blendIsDefault = !parseBlendState(commonPassData.blendDesc, techniqueNode);
		commonPassData.rasterizerIsDefault = !parseRasterizerState(commonPassData.rasterizerDesc, techniqueNode);
		commonPassData.depthStencilIsDefault = !parseDepthStencilState(commonPassData.depthStencilDesc, techniqueNode);

		UINT32 nextPassIdx = 0;
		Map<UINT32, PassPtr, std::greater<UINT32>> passes;
		for (auto& passNode : passNodes)
		{
			PassData passDataCopy = commonPassData;

			UINT32 passIdx = nextPassIdx;
			PassPtr pass = parsePass(passNode, codeBlocks, passDataCopy, renderAPI, language, passIdx);

			if (pass != nullptr)
				passes[passIdx] = pass;

			nextPassIdx = std::max(nextPassIdx, passIdx) + 1;
		}

		Vector<PassPtr> orderedPasses;
		for (auto& KVP : passes)
			orderedPasses.push_back(KVP.second);

		if (orderedPasses.size() > 0)
			return Technique::create(renderAPI, renderer, orderedPasses);

		return nullptr;
	}

	void BSLFXCompiler::parseParameters(SHADER_DESC& desc, ASTFXNode* parametersNode)
	{
		if (parametersNode == nullptr || parametersNode->type != NT_Parameters)
			return;
		for (int i = 0; i < parametersNode->options->count; i++)
		{
			NodeOption* option = &parametersNode->options->entries[i];

			if (option->type != OT_Parameter)
				continue;

			ASTFXNode* parameter = option->value.nodePtr;

			String name;
			String alias;

			UINT32 typeId = 0;
			bool isObjType = false;
			StringID semantic;
			SamplerStatePtr samplerState = nullptr;
			float defaultValue[16];
			HTexture defaultTexture;
			bool hasDefaultValue = false;
			for (int j = 0; j < parameter->options->count; j++)
			{
				NodeOption* paramOption = &parameter->options->entries[j];

				switch (paramOption->type)
				{
				case OT_Identifier:
					name = paramOption->value.strValue;
					break;
				case OT_Alias:
					alias = removeQuotes(paramOption->value.strValue);
					break;
				case OT_ParamType:
					parseParamType((ParamType)paramOption->value.intValue, isObjType, typeId);
					break;
				case OT_ParamValue:
					memcpy(defaultValue, paramOption->value.matrixValue, sizeof(defaultValue));
					hasDefaultValue = true;
					break;
				case OT_ParamStrValue:
				{
					String defaultTextureName = removeQuotes(paramOption->value.strValue);
					defaultTexture = getBuiltinTexture(defaultTextureName);
					hasDefaultValue = true;
				}
					break;
				case OT_Auto:
					semantic = removeQuotes(paramOption->value.strValue);
					break;
				case OT_SamplerState:
					samplerState = parseSamplerState(paramOption->value.nodePtr);
					break;
				}
			}

			if (name.empty())
				continue;

			auto addParameter = [&](const String& paramName, const String& gpuVarName)
			{
				if (isObjType)
				{
					GpuParamObjectType objType = (GpuParamObjectType)typeId;

					if (Shader::isSampler(objType))
					{
						if(hasDefaultValue)
							desc.addParameter(paramName, gpuVarName, objType, samplerState, semantic);
						else
							desc.addParameter(paramName, gpuVarName, objType, semantic);
					}
					else if(Shader::isTexture(objType))
					{
						if(hasDefaultValue)
							desc.addParameter(paramName, gpuVarName, objType, defaultTexture, semantic);
						else
							desc.addParameter(paramName, gpuVarName, objType, semantic);
					}
					else
						desc.addParameter(paramName, gpuVarName, objType, semantic);
				}
				else
				{
					if (hasDefaultValue)
						desc.addParameter(paramName, gpuVarName, (GpuParamDataType)typeId, semantic, 1, 0, (UINT8*)defaultValue);
					else
						desc.addParameter(paramName, gpuVarName, (GpuParamDataType)typeId, semantic);
				}
			};

			addParameter(name, name);

			if (!alias.empty())
				addParameter(name, alias);
		}
	}

	void BSLFXCompiler::parseBlocks(SHADER_DESC& desc, ASTFXNode* blocksNode)
	{
		if (blocksNode == nullptr || blocksNode->type != NT_Blocks)
			return;

		for (int i = 0; i < blocksNode->options->count; i++)
		{
			NodeOption* option = &blocksNode->options->entries[i];

			if (option->type != OT_Block)
				continue;

			ASTFXNode* parameter = option->value.nodePtr;

			String name;
			bool shared = false;
			GpuParamBlockUsage usage = GPBU_STATIC;
			StringID semantic;

			for (int j = 0; j < parameter->options->count; j++)
			{
				NodeOption* paramOption = &parameter->options->entries[j];

				switch (paramOption->type)
				{
				case OT_Identifier:
					name = paramOption->value.strValue;
					break;
				case OT_Shared:
					shared = paramOption->value.intValue > 0;
					break;
				case OT_Usage:
					usage = parseBlockUsage((BufferUsageValue)paramOption->value.intValue);
					break;
				case OT_Auto:
					semantic = removeQuotes(paramOption->value.strValue);
					break;
				}
			}

			if (name.empty())
				continue;

			desc.setParamBlockAttribs(name, shared, usage, semantic);
		}
	}

	BSLFXCompileResult BSLFXCompiler::parseShader(const String& name, ParseState* parseState, Vector<CodeBlock>& codeBlocks)
	{
		BSLFXCompileResult output;

		if (parseState->rootNode == nullptr || parseState->rootNode->type != NT_Shader)
		{
			output.errorMessage = "Root not is null or not a shader.";
			return output;
		}

		// Merge all include ASTs
		std::function<ParseState*(ParseState*, Vector<CodeBlock>&)> parseIncludes = 
			[&](ParseState* parentParseState, Vector<CodeBlock>& parentCodeBlocks) -> ParseState*
		{
			ASTFXNode* node = parentParseState->rootNode;
			Vector<tuple<ParseState*, Vector<CodeBlock>>> toMerge;

			for (int i = 0; i < node->options->count; i++)
			{
				NodeOption* option = &node->options->entries[i];

				if (option->type == OT_Include)
				{
					String includePath = removeQuotes(option->value.strValue);
					HShaderInclude include = ShaderManager::instance().findInclude(includePath);

					if (include != nullptr)
						include.blockUntilLoaded();

					if (include.isLoaded())
					{
						String includeSource = include->getString();

						ParseState* includeParseState = parseStateCreate();
						Vector<CodeBlock> includeCodeBlocks = parseCodeBlocks(includeSource);
						parseFX(includeParseState, includeSource.c_str());

						if (includeParseState->hasError > 0)
						{
							output.errorMessage = includeParseState->errorMessage;
							output.errorLine = includeParseState->errorLine;
							output.errorColumn = includeParseState->errorColumn;

							parseStateDelete(includeParseState);
							return nullptr;
						}
						else
						{
							ParseState* combinedIncludeParseState = parseIncludes(includeParseState, includeCodeBlocks);

							toMerge.push_back(make_tuple(combinedIncludeParseState, includeCodeBlocks));
						}
					}
					else
					{
						output.errorMessage = "Cannot find shader include file: " + includePath;
						return nullptr;
					}
				}
			}

			UINT32 size = (UINT32)toMerge.size();
			if (size > 0)
			{
				Vector<CodeBlock>& outputCodeBlocks = get<1>(toMerge[0]);
				for (UINT32 i = 1; i < size; i++)
				{
					mergeAST(get<0>(toMerge[0]), get<0>(toMerge[i])->rootNode, (UINT32)outputCodeBlocks.size());

					const Vector<CodeBlock>& curCodeBlocks = get<1>(toMerge[i]);
					for (auto& codeBlock : curCodeBlocks)
						outputCodeBlocks.push_back(codeBlock);

					parseStateDelete(get<0>(toMerge[i]));
				}

				mergeAST(get<0>(toMerge[0]), node, (UINT32)outputCodeBlocks.size());
				for (auto& codeBlock : parentCodeBlocks)
					outputCodeBlocks.push_back(codeBlock);

				parseStateDelete(parentParseState);

				parentCodeBlocks = outputCodeBlocks;
				return get<0>(toMerge[0]);
			}
			else
				return parentParseState;
		};

		parseState = parseIncludes(parseState, codeBlocks);
		if (parseState == nullptr) // Error
			return output;

		Vector<String> topLevelIncludes;

		SHADER_DESC shaderDesc;
		Vector<TechniquePtr> techniques;

		for (int i = 0; i < parseState->rootNode->options->count; i++)
		{
			NodeOption* option = &parseState->rootNode->options->entries[i];

			switch (option->type)
			{
			case OT_Include:
			{
				String includePath = removeQuotes(option->value.strValue);
				topLevelIncludes.push_back(includePath);
			}
				break;
			case OT_Separable:
				shaderDesc.separablePasses = option->value.intValue > 1;
				break;
			case OT_Queue:
				shaderDesc.queuePriority = option->value.intValue;
				break;
			case OT_Priority:
				shaderDesc.queuePriority = option->value.intValue;
				break;
			case OT_Transparent:
				shaderDesc.flags |= (UINT32)ShaderFlags::Transparent;
				break;
			case OT_Technique:
			{
				TechniquePtr technique = parseTechnique(option->value.nodePtr, codeBlocks);
				if (technique != nullptr)
					techniques.push_back(technique);

				break;
			}
			case OT_Parameters:
				parseParameters(shaderDesc, option->value.nodePtr);
				break;
			case OT_Blocks:
				parseBlocks(shaderDesc, option->value.nodePtr);
				break;
			}
		}

		parseStateDelete(parseState);

		output.shader = Shader::_createPtr(name, shaderDesc, techniques);
		output.shader->setIncludeFiles(topLevelIncludes);

		return output;
	}

	String BSLFXCompiler::removeQuotes(const char* input)
	{
		UINT32 len = (UINT32)strlen(input);
		String output(len - 2, ' ');

		for (UINT32 i = 0; i < (len - 2); i++)
			output[i] = input[i + 1];

		return output;
	}

	GpuProgramProfile BSLFXCompiler::getProfile(const StringID& renderAPI, GpuProgramType type)
	{
		StringID target = renderAPI;
		if (target == RenderAPIAny)
			target = RenderAPICore::instance().getName();

		if (target == RenderAPIDX11 || target == RenderAPIOpenGL)
		{
			switch (type)
			{
			case GPT_VERTEX_PROGRAM:
				return GPP_VS_5_0;
			case GPT_FRAGMENT_PROGRAM:
				return GPP_FS_5_0;
			case GPT_GEOMETRY_PROGRAM:
				return GPP_GS_5_0;
			case GPT_HULL_PROGRAM:
				return GPP_HS_5_0;
			case GPT_DOMAIN_PROGRAM:
				return GPP_DS_5_0;
			case GPT_COMPUTE_PROGRAM:
				return GPP_CS_5_0;
			}
		}
		else if (target == RenderAPIDX9)
		{
			switch (type)
			{
			case GPT_VERTEX_PROGRAM:
				return GPP_VS_3_0;
			case GPT_FRAGMENT_PROGRAM:
				return GPP_FS_3_0;
			}
		}

		return GPP_NONE;
	}

	HTexture BSLFXCompiler::getBuiltinTexture(const String& name)
	{
		if (StringUtil::compare(name, String("white"), false) == 0)
			return BuiltinResources::getTexture(BuiltinTexture::White);
		else if (StringUtil::compare(name, String("black"), false) == 0)
			return BuiltinResources::getTexture(BuiltinTexture::Black);
		else if (StringUtil::compare(name, String("normal"), false) == 0)
			return BuiltinResources::getTexture(BuiltinTexture::Normal);

		return HTexture();
	}
}