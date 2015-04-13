#include "BsSLFXCompiler.h"
#include "BsGpuProgram.h"
#include <regex>
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsSamplerState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsBlendState.h"
#include "BsRenderAPI.h"
#include "BsDebug.h"

extern "C" {
#include "BsMMAlloc.h"
#include "BsParserFX.h"
#include "BsLexerFX.h"
}

namespace BansheeEngine
{
	// Print out the FX AST, only for debug purposes
	void SLFXDebugPrint(ASTFXNode* node, String indent)
	{
		LOGWRN(indent + "NODE " + toString(node->type));

		for (int i = 0; i < node->options->count; i++)
		{
			OptionDataType odt = OPTION_LOOKUP[(int)node->options->entries[i].type].dataType;
			if (odt == ODT_Complex)
			{
				LOGWRN(indent + toString(i) + ". " + toString(node->options->entries[i].type));
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

			LOGWRN(indent + toString(i) + ". " + toString(node->options->entries[i].type) + " = " + value);
		}
	}

	ShaderPtr BSLFXCompiler::compile(const String& source)
	{
		String parsedSource = source;

		ParseState* parseState = parseStateCreate();
		Vector<CodeBlock> codeBlocks = parseCodeBlocks(parsedSource);
		parseFX(parseState, parsedSource.c_str());

		ShaderPtr output;
		if (parseState->hasError > 0)
		{
			LOGERR("Error while parsing a Shader: " + String(parseState->errorMessage) + ". Location: " +
				toString(parseState->errorLine) + " (" + toString(parseState->errorColumn) + ")");
		}
		else
		{
			output = parseShader("Shader", parseState->rootNode, codeBlocks);

			// Only enable for debug purposes
			//SLFXDebugPrint(parseState->rootNode, "");
		}

		parseStateDelete(parseState);

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
		std::regex pattern = std::regex(R"((Vertex|Fragment|Geometry|Hull|Domain|Compute)\s*=\s*\{)");
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
				newBlock.type = GPT_VERTEX_PROGRAM;
			else if (type == "Fragment")
				newBlock.type = GPT_FRAGMENT_PROGRAM;
			else if (type == "Geometry")
				newBlock.type = GPT_GEOMETRY_PROGRAM;
			else if (type == "Hull")
				newBlock.type = GPT_HULL_PROGRAM;
			else if (type == "Domain")
				newBlock.type = GPT_DOMAIN_PROGRAM;
			else if (type == "Compute")
				newBlock.type = GPT_COMPUTE_PROGRAM;

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
			language = "hlsl";
		}
		else if (name == "GLSL")
		{
			renderAPI = RenderAPIOpenGL;
			language = "glsl";
		}
		else
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

	HBlendState BSLFXCompiler::parseBlendState(ASTFXNode* passNode)
	{
		if (passNode == nullptr || passNode->type != NT_Pass)
			return HBlendState();

		BLEND_STATE_DESC desc;
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

		if (default)
			return HBlendState();

		return BlendState::create(desc);
	}

	HRasterizerState BSLFXCompiler::parseRasterizerState(ASTFXNode* passNode)
	{
		if (passNode == nullptr || passNode->type != NT_Pass)
			return HRasterizerState();

		RASTERIZER_STATE_DESC desc;
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

		if (default)
			return HRasterizerState();

		return RasterizerState::create(desc);
	}

	HDepthStencilState BSLFXCompiler::parseDepthStencilState(ASTFXNode* passNode)
	{
		if (passNode == nullptr || passNode->type != NT_Pass)
			return HDepthStencilState();

		DEPTH_STENCIL_STATE_DESC desc;
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

		if (default)
			return HDepthStencilState();

		return DepthStencilState::create(desc);
	}

	HSamplerState BSLFXCompiler::parseSamplerState(ASTFXNode* samplerStateNode)
	{
		if (samplerStateNode == nullptr || samplerStateNode->type != NT_SamplerState)
			return HSamplerState();

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
			return HSamplerState();

		return SamplerState::create(desc);
	}

	PassPtr BSLFXCompiler::parsePass(ASTFXNode* passNode, const Vector<CodeBlock>& codeBlocks, const Vector<String>& includes, const StringID& renderAPI, const String& language)
	{
		if (passNode == nullptr || passNode->type != NT_Pass)
			return nullptr;

		PASS_DESC passDesc;

		passDesc.blendState = parseBlendState(passNode);
		passDesc.rasterizerState = parseRasterizerState(passNode);
		passDesc.depthStencilState = parseDepthStencilState(passNode);

		for (int i = 0; i < passNode->options->count; i++)
		{
			NodeOption* option = &passNode->options->entries[i];

			switch (option->type)
			{
			case OT_StencilRef:
				passDesc.stencilRefValue = option->value.intValue;
				break;
			case OT_Code:
			{
				ASTFXNode* codeNode = option->value.nodePtr;

				if (codeNode != nullptr && codeNode->type == NT_Code)
				{
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
						case GPT_VERTEX_PROGRAM:
							passDesc.vertexProgram = GpuProgram::create(codeBlock.code, "main", language,
								codeBlock.type, getProfile(renderAPI, codeBlock.type));
							break;
						case GPT_FRAGMENT_PROGRAM:
							passDesc.fragmentProgram = GpuProgram::create(codeBlock.code, "main", language,
								codeBlock.type, getProfile(renderAPI, codeBlock.type));
							break;
						case GPT_GEOMETRY_PROGRAM:
							passDesc.geometryProgram = GpuProgram::create(codeBlock.code, "main", language,
								codeBlock.type, getProfile(renderAPI, codeBlock.type));
							break;
						case GPT_HULL_PROGRAM:
							passDesc.hullProgram = GpuProgram::create(codeBlock.code, "main", language,
								codeBlock.type, getProfile(renderAPI, codeBlock.type));
							break;
						case GPT_DOMAIN_PROGRAM:
							passDesc.domainProgram = GpuProgram::create(codeBlock.code, "main", language,
								codeBlock.type, getProfile(renderAPI, codeBlock.type));
							break;
						case GPT_COMPUTE_PROGRAM:
							passDesc.computeProgram = GpuProgram::create(codeBlock.code, "main", language,
								codeBlock.type, getProfile(renderAPI, codeBlock.type));
							break;
						}
					}
				}
			}
				break;
			}
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
		Vector<String> includes; // TODO - Need to figure out what to do with these

		for (int i = 0; i < techniqueNode->options->count; i++)
		{
			NodeOption* option = &techniqueNode->options->entries[i];

			switch (option->type)
			{
			case OT_Pass:
				passNodes.push_back(option->value.nodePtr);
				break;
			case OT_Include:
			{
				String include = removeQuotes(option->value.strValue);
				includes.push_back(include);
			}
				break;
			case OT_Renderer:
				renderer = parseRenderer(removeQuotes(option->value.strValue));
				break;
			case OT_Language:
				parseLanguage(removeQuotes(option->value.strValue), renderAPI, language);
				break;
			}
		}

		Vector<PassPtr> passes;
		for (auto& passNode : passNodes)
		{
			PassPtr pass = parsePass(passNode, codeBlocks, includes, renderAPI, language);
			if (pass != nullptr)
				passes.push_back(pass);
		}

		if (passes.size() > 0)
			return Technique::create(renderAPI, renderer, passes);

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

			float defaultValue[16];
			UINT32 typeId;
			bool isObjType = false;
			StringID semantic;

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
				case OT_ParamValue:
					memcpy(defaultValue, paramOption->value.matrixValue, sizeof(defaultValue));
					break;
				case OT_ParamType:
					parseParamType((ParamType)paramOption->value.intValue, isObjType, typeId);
					break;
				case OT_Auto:
					semantic = removeQuotes(paramOption->value.strValue);
					break;
				case OT_SamplerState:
					HSamplerState samplerState = parseSamplerState(paramOption->value.nodePtr);
					// TODO - How to deal with sampler-state default value?
					break;
				}
			}

			if (name.empty())
				continue;

			if (isObjType)
				desc.addParameter(name, name, (GpuParamObjectType)typeId, semantic);
			else
				desc.addParameter(name, name, (GpuParamDataType)typeId, semantic); // TODO - Add default value

			if (!alias.empty())
			{
				if (isObjType)
					desc.addParameter(name, alias, (GpuParamObjectType)typeId, semantic);
				else
					desc.addParameter(name, alias, (GpuParamDataType)typeId, semantic); // TODO - Add default value
			}
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
			bool shared;
			GpuParamBlockUsage usage;
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

	ShaderPtr BSLFXCompiler::parseShader(const String& name, ASTFXNode* rootNode, const Vector<CodeBlock>& codeBlocks)
	{
		SHADER_DESC shaderDesc;
		Vector<TechniquePtr> techniques;

		if (rootNode->type == NT_Shader)
		{
			for (int i = 0; i < rootNode->options->count; i++)
			{
				NodeOption* option = &rootNode->options->entries[i];

				switch (option->type)
				{
				case OT_Separable:
					shaderDesc.separablePasses = option->value.intValue > 1;
					break;
				case OT_Queue:
					shaderDesc.queuePriority = option->value.intValue;
					break;
				case OT_Priority:
					shaderDesc.queuePriority = option->value.intValue;
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
		}

		return Shader::_createPtr(name, shaderDesc, techniques);
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
}