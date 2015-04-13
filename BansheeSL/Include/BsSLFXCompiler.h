#pragma once

#include "BsSLPrerequisites.h"
#include "BsShader.h"
#include "BsGpuProgram.h"

extern "C" {
#include "BsASTFX.h"
}

namespace BansheeEngine
{
	/**
	 * @brief	Transforms a source file written in BSL FX syntax into a Shader object.
	 */
	class BSLFXCompiler
	{
		/**
		 * @brief	Represents a block of code written in a GPU program language for
		 *			a specific GPU program type. (i.e. non-FX code)
		 */
		struct CodeBlock
		{
			GpuProgramType type;
			String code;
		};

	public:
		/**
		 * @brief	Transforms a source file written in BSL FX syntax into a Shader object.
		 *
		 * @note	If error occurs a nullptr will be returned and the error will be logged.
		 */
		static ShaderPtr compile(const String& source);

	private:
		/**
		 * @brief	Converts the provided source into an abstract syntax tree using the
		 *			lexer & parser for BSL FX syntax.
		 */
		static void parseFX(ParseState* parseState, const char* source);

		/**
		 * @brief	Retrieves non-FX code blocks (i.e. actual shader code) from the source code and
		 *			removes them from the input so all that remains is a pure FX code. Found blocks
		 *			are returned so they may be compiled using their appropriate compiler.
		 */
		static Vector<CodeBlock> parseCodeBlocks(String& source);

		/**
		 * @brief	Converts FX renderer name into an in-engine renderer identifier.
		 */
		static StringID parseRenderer(const String& name);

		/**
		 * @brief	Converts FX language into an in-engine shader language (e.g. hlsl, glsl) and
		 *			a rendering API that supports the provided language.
		 */
		static void parseLanguage(const String& name, StringID& renderAPI, String& language);

		/**
		 * @brief	Maps FX buffer usage enum into in-engine param block usage.
		 */
		static GpuParamBlockUsage parseBlockUsage(BufferUsageValue usage);

		/**
		 * @brief	Maps FX filter mode enum into in-engine filter mode.
		 */
		static UINT32 parseFilterMode(FilterValue filter);

		/**
		 * @brief	Maps FX comparison function enum into in-engine compare function.
		 */
		static CompareFunction parseCompFunc(CompFuncValue compFunc);

		/**
		 * @brief	Maps FX addressing mode enum into in-engine addressing mode.
		 */
		static TextureAddressingMode parseAddrMode(AddrModeValue addrMode);

		/**
		 * @brief	Maps FX operation to in-engine blend factor.
		 */
		static BlendFactor parseBlendFactor(OpValue factor);

		/**
		 * @brief	Maps FX blend operation to in-engine blend operation.
		 */
		static BlendOperation parseBlendOp(BlendOpValue op);

		/**
		 * @brief	Maps FX parameter type to in-engine shader parameter.
		 *
		 * @param	type		Input FX parameter type.
		 * @param	isObjType	Output parameter signaling whether the in-engine parameter is
		 *						a data or an object type.
		 * @param	typeId		Type ID corresponding to a value of in-game GpuParamDataType or GpuParamObjectType
		 *						enum (depending on "isObjType").
		 */
		static void parseParamType(ParamType type, bool& isObjType, UINT32& typeId);

		/**
		 * @brief	Maps FX operation to in-engine stencil operation.
		 */
		static StencilOperation parseStencilOp(OpValue op);
		
		/**
		 * @brief	Maps FX cull mode enum to in-engine cull mode.
		 */
		static CullingMode parseCullMode(CullModeValue cm);

		/**
		 * @brief	Maps FX fill mode enum to in-engine fill mode.
		 */
		static PolygonMode parseFillMode(FillModeValue fm);

		/**
		 * @brief	Populates the front facing operation portion of the depth-stencil state descriptor
		 *			from the provided stencil-op AST node.
		 */
		static void parseStencilFront(DEPTH_STENCIL_STATE_DESC& desc, ASTFXNode* stencilOpNode);

		/**
		 * @brief	Populates the back backing operation portion of the depth-stencil state descriptor
		 *			from the provided stencil-op AST node.
		 */
		static void parseStencilBack(DEPTH_STENCIL_STATE_DESC& desc, ASTFXNode* stencilOpNode);

		/**
		 * @brief	Populates the addressing mode portion of the sampler state descriptor for U/V/W axes from
		 *			the provided addressing mode AST node.
		 */
		static void parseAddrMode(SAMPLER_STATE_DESC& desc, ASTFXNode* addrModeNode);

		/**
		 * @brief	Populates the color (RGB) portion of the blend state descriptor from the provided
		 *			blend definition AST node.
		 */
		static void parseColorBlendDef(RENDER_TARGET_BLEND_STATE_DESC& desc, ASTFXNode* blendDefNode);

		/**
		 * @brief	Populates the alpha portion of the blend state descriptor from the provided
		 *			blend definition AST node.
		 */
		static void parseAlphaBlendDef(RENDER_TARGET_BLEND_STATE_DESC& desc, ASTFXNode* blendDefNode);

		/**
		 * @brief	Populates blend state descriptor for a single render target from the provided
		 *			AST node. Which target gets updated depends on the index set in the AST node.
		 */
		static void parseRenderTargetBlendState(BLEND_STATE_DESC& desc, ASTFXNode* targetNode);

		/**
		 * @brief	Parses the blend state AST node and outputs a blend state object, or an invalid
		 *			handle in case AST node is empty. 
		 */
		static HBlendState parseBlendState(ASTFXNode* passNode);

		/**
		 * @brief	Parses the rasterizer state AST node and outputs a rasterizer state object, or an invalid
		 *			handle in case AST node is empty. 
		 */
		static HRasterizerState parseRasterizerState(ASTFXNode* passNode);

		/**
		 * @brief	Parses the depth-stencil state AST node and outputs a depth-stencil state object, or an invalid
		 *			handle in case AST node is empty. 
		 */
		static HDepthStencilState parseDepthStencilState(ASTFXNode* passNode);

		/**
		 * @brief	Parses the sampler state AST node and outputs a sampler state object, or an invalid
		 *			handle in case AST node is empty. 
		 */
		static HSamplerState parseSamplerState(ASTFXNode* samplerStateNode);

		/**
		 * @brief	Parses the pass AST node and generates a single pass object. Returns null
		 *			if no pass can be parsed. This method will generate any child state objects and
		 *			compile any child GPU programs.
		 *
		 * @param	passNode		Node to parse.
		 * @param	codeBlocks		GPU program source code retrieved from "parseCodeBlocks".
		 * @param	includes		Files paths relative to the current executable that will be injected
		 *							into GPU program code before compilation.
		 * @param	renderAPI		API to use for compiling the GPU programs.
		 * @param	language		GPU program language to use for parsing the provided code blocks.
		 */
		static PassPtr parsePass(ASTFXNode* passNode, const Vector<CodeBlock>& codeBlocks, const Vector<String>& includes, const StringID& renderAPI, const String& language);

		/**
		 * @brief	Parses the technique AST node and generates a single technique object. Returns null
		 *			if no technique can be parsed.
		 *
		 * @param	techniqueNode	Node to parse.
		 * @param	codeBlocks		GPU program source code retrieved from "parseCodeBlocks".
		 */
		static TechniquePtr parseTechnique(ASTFXNode* techniqueNode, const Vector<CodeBlock>& codeBlocks);

		/**
		 * @brief	Parses the parameters AST node and populates the shader descriptor with information
		 *			about GPU program parameters and their default values.
		 */
		static void parseParameters(SHADER_DESC& desc, ASTFXNode* parametersNode);

		/**
		 * @brief	Parses the blocks AST node and populates the shader descriptor with information
		 *			about GPU program parameter blocks.
		 */
		static void parseBlocks(SHADER_DESC& desc, ASTFXNode* blocksNode);

		/**
		 * @brief	Parses the AST node hierarchy and generates a shader object.
		 *
		 * @param	name		Optional name for the shader.
		 * @param	rootNode	Root node of the AST hierarchy retrieved from "parseFX".
		 * @param	codeBlocks	GPU program source code retrieved from "parseCodeBlocks".
		 *
		 * @return	A generated shader object, or a nullptr if shader was invalid.
		 */
		static ShaderPtr parseShader(const String& name, ASTFXNode* rootNode, const Vector<CodeBlock>& codeBlocks);

		/**
		 * @brief	Converts a null-terminated string into a standard string, and eliminates quotes that are assumed
		 *			to be at the first and last index.
		 */
		static String removeQuotes(const char* input);

		/**
		 * @brief	Retrieves a GPU program profile to use with the specified API and GPU program type.
		 */
		static GpuProgramProfile getProfile(const StringID& renderAPI, GpuProgramType type);
	};
}