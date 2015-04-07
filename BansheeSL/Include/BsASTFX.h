#ifndef __ASTFX_H__
#define __ASTFX_H__

#include <stdlib.h>
#include <string.h>

typedef enum tagNodeType NodeType;
typedef enum tagOptionType OptionType;
typedef enum tagOptionDataType OptionDataType;
typedef enum tagParamType ParamType;
typedef struct tagParseState ParseState;
typedef struct tagOptionInfo OptionInfo;
typedef union tagOptionData OptionData;
typedef struct tagNodeOptions NodeOptions;
typedef struct tagNodeOption NodeOption;
typedef struct tagASTFXNode ASTFXNode;
typedef struct tagNodeLink NodeLink;

enum tagNodeType
{
	NT_Shader,
	NT_Technique,
	NT_Parameters,
	NT_Blocks,
	NT_Pass,
	NT_StencilOp,
	NT_Target,
	NT_BlendDef,
	NT_SamplerState,
	NT_AddrMode,
	NT_Parameter,
	NT_Block
};

enum tagOptionType
{
	OT_None = 0,
	OT_Separable,
	OT_Priority,
	OT_Queue,
	OT_Technique,
	OT_Renderer,
	OT_Language,
	OT_Include,
	OT_Pass,
	OT_FillMode,
	OT_CullMode,
	OT_DepthBias,
	OT_SDepthBias,
	OT_DepthClip,
	OT_Scissor,
	OT_Multisample,
	OT_AALine,
	OT_DepthRead,
	OT_DepthWrite,
	OT_CompareFunc,
	OT_Stencil,
	OT_StencilReadMask,
	OT_StencilWriteMask,
	OT_StencilOpFront,
	OT_StencilOpBack,
	OT_PassOp,
	OT_Fail,
	OT_ZFail,
	OT_AlphaToCoverage,
	OT_IndependantBlend,
	OT_Target,
	OT_Index,
	OT_Blend,
	OT_Color,
	OT_Alpha,
	OT_WriteMask,
	OT_Source,
	OT_Dest,
	OT_Op,
	OT_AddrMode,
	OT_MinFilter,
	OT_MagFilter,
	OT_MipFilter,
	OT_MaxAniso,
	OT_MipBias,
	OT_MipMin,
	OT_MipMax,
	OT_BorderColor,
	OT_U,
	OT_V,
	OT_W,
	OT_Alias,
	OT_Auto,
	OT_Shared,
	OT_Usage,
	OT_ParamType,
	OT_Identifier,
	OT_ParamValue,
	OT_Parameters,
	OT_Blocks,
	OT_Parameter,
	OT_Block,
	OT_SamplerState
};

enum tagOptionDataType
{
	ODT_Int,
	ODT_Float,
	ODT_Bool,
	ODT_String,
	ODT_Complex,
	ODT_Matrix
};

enum tagParamType
{
	PT_Float, PT_Float2, PT_Float3, PT_Float4,
	PT_Mat2x2, PT_Mat2x3, PT_Mat2x4,
	PT_Mat3x2, PT_Mat3x3, PT_Mat3x4,
	PT_Mat4x2, PT_Mat4x3, PT_Mat4x4,
	PT_Sampler1D, PT_Sampler2D, PT_Sampler3D, PT_SamplerCUBE, PT_Sampler2DMS,
	PT_Texture1D, PT_Texture2D, PT_Texture3D, PT_TextureCUBE, PT_Texture2DMS,
	PT_ByteBuffer, PT_StructBuffer, PT_ByteBufferRW, PT_StructBufferRW,
	PT_TypedBufferRW, PT_AppendBuffer, PT_ConsumeBuffer
};

struct tagNodeLink
{
	ASTFXNode* node;
	NodeLink* next;
};

struct tagParseState
{
	ASTFXNode* rootNode;
	ASTFXNode* topNode;
	void* memContext;

	NodeLink* nodeStack;
};

struct tagOptionInfo
{
	OptionType type;
	OptionDataType dataType;
};

union tagOptionData
{
	int intValue;
	float floatValue;
	const char* strValue;
	float matrixValue[16];
	ASTFXNode* nodePtr;
};

struct tagNodeOption
{
	OptionType type;
	OptionData value;
};

struct tagNodeOptions
{
	NodeOption* entries;

	int count;
	int bufferSize;
};

struct tagASTFXNode
{
	NodeType type;
	NodeOptions* options;
};

OptionInfo OPTION_LOOKUP[];

NodeOptions* nodeOptionsCreate(void* context);
void nodeOptionDelete(NodeOption* option);
void nodeOptionsDelete(NodeOptions* options);
void nodeOptionsResize(void* context, NodeOptions* options, int size);
void nodeOptionsGrowIfNeeded(void* context, NodeOptions* options);
void nodeOptionsAdd(void* context, NodeOptions* options, const NodeOption* option);

ASTFXNode* nodeCreate(void* context, NodeType type);
void nodeDelete(ASTFXNode* node);

void nodePush(ParseState* parseState, ASTFXNode* node);
void nodePop(ParseState* parseState);

ParseState* parseStateCreate();
void parseStateDelete(ParseState* parseState);

#endif