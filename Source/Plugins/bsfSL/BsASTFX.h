//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#ifndef __ASTFX_H__
#define __ASTFX_H__

#include <stdlib.h>
#include <string.h>

enum tagNodeType
{
	NT_Root,
	NT_Options,
	NT_Shader,
	NT_SubShader,
	NT_Mixin,
	NT_Pass,
	NT_Blend,
	NT_Raster,
	NT_Depth,
	NT_Stencil,
	NT_Target,
	NT_StencilOp,
	NT_BlendDef,
	NT_Tags,
	NT_Code,
	NT_Variations,
	NT_Variation,
	NT_VariationOption,
	NT_Attributes
};

enum tagOptionType
{
	OT_None = 0,
	OT_Options,
	OT_Separable,
	OT_Priority,
	OT_Sort,
	OT_Transparent,
	OT_Shader,
	OT_SubShader,
	OT_Mixin,
	OT_Raster,
	OT_Depth,
	OT_Stencil,
	OT_Blend,
	OT_FeatureSet,
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
	OT_Enabled,
	OT_Color,
	OT_Alpha,
	OT_WriteMask,
	OT_Source,
	OT_Dest,
	OT_Op,
	OT_Identifier,
	OT_Code,
	OT_StencilRef,
	OT_Tags,
	OT_TagValue,
	OT_Variations,
	OT_Variation,
	OT_VariationValue,
	OT_Forward,
	OT_Attributes,
	OT_AttrName,
	OT_VariationOption,
	OT_AttrShow,
	OT_Count
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

enum tagFillModeValue
{
	FMV_Wire, FMV_Solid
};

enum tagCullAndSortModeValue
{
	CASV_None, CASV_CW, CASV_CCW, CASV_FrontToBack, CASV_BackToFront
};

enum tagCompFuncValue
{
	CFV_Fail, CFV_Pass, CFV_LT, CFV_LTE,
	CFV_EQ, CFV_NEQ, CFV_GTE, CFV_GT
};

enum tagOpValue
{
	OV_Keep, OV_Zero, OV_Replace, OV_Incr, OV_Decr,
	OV_IncrWrap, OV_DecrWrap, OV_Invert, OV_One, OV_DestColor,
	OV_SrcColor, OV_InvDestColor, OV_InvSrcColor, OV_DestAlpha,
	OV_SrcAlpha, OV_InvDestAlpha, OV_InvSrcAlpha
};

enum tagBlendOpValue
{
	BOV_Add, BOV_Subtract, BOV_RevSubtract,
	BOV_Min, BOV_Max
};

enum tagRawCodeType
{
	RCT_CodeBlock,
	RCT_SubShaderBlock,
	RCT_Count
};

enum tagConditionalOp
{
	CO_None, CO_Equals, CO_NotEquals, CO_Lesser, CO_Greater, CO_LesserEqual, CO_GreaterEqual
};

typedef enum tagNodeType NodeType;
typedef enum tagOptionType OptionType;
typedef enum tagOptionDataType OptionDataType;
typedef struct tagParseState ParseState;
typedef struct tagOptionInfo OptionInfo;
typedef union tagOptionData OptionData;
typedef struct tagNodeOptions NodeOptions;
typedef struct tagNodeOption NodeOption;
typedef struct tagASTFXNode ASTFXNode;
typedef struct tagNodeLink NodeLink;
typedef struct tagIncludeData IncludeData;
typedef struct tagIncludeLink IncludeLink;
typedef struct tagConditionalData ConditionalData;
typedef struct tagRawCode RawCode;
typedef struct tagDefineEntry DefineEntry;
typedef enum tagFillModeValue FillModeValue;
typedef enum tagCullAndSortModeValue CullAndSortModeValue;
typedef enum tagCompFuncValue CompFuncValue;
typedef enum tagOpValue OpValue;
typedef enum tagBlendOpValue BlendOpValue;
typedef enum tagRawCodeType RawCodeType;
typedef enum tagConditionalOp ConditionalOp;

struct tagNodeLink
{
	ASTFXNode* node;
	NodeLink* next;
};

struct tagIncludeData
{
	char* filename;
	char* buffer;
};

struct tagIncludeLink
{
	IncludeData* data;
	IncludeLink* next;
};

struct tagConditionalData
{
	char* name;
	int selfEnabled;
	int enabled;
	ConditionalOp op;
	char* value;

	ConditionalData* next;
};

struct tagRawCode
{
	char* code;
	int index;
	int size;
	int capacity;

	RawCode* next;
};

struct tagDefineEntry
{
	char* name;
	char* expr;
};

struct tagParseState
{
	ASTFXNode* rootNode;
	ASTFXNode* topNode;
	void* memContext;

	int hasError;
	int errorLine;
	int errorColumn;
	const char* errorMessage;
	char* errorFile;

	NodeLink* nodeStack;
	IncludeLink* includeStack;
	IncludeLink* includes;
	RawCode* rawCodeBlock[RCT_Count];
	int numRawCodeBlocks[RCT_Count];
	int numOpenBrackets;

	DefineEntry* defines;
	int numDefines;
	int defineCapacity;
	ConditionalData* conditionalStack;
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
	int intVectorValue[4];
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

extern OptionInfo OPTION_LOOKUP[OT_Count];

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

void beginCodeBlock(ParseState* parseState, RawCodeType type);
void appendCodeBlock(ParseState* parseState, RawCodeType type, const char* value, int size);
int getCodeBlockIndex(ParseState* parseState, RawCodeType type);

void addDefine(ParseState* parseState, const char* value);
void addDefineExpr(ParseState* parseState, const char* value);
int hasDefine(ParseState* parseState, const char* value);
void removeDefine(ParseState* parseState, const char* value);

int pushConditionalDef(ParseState* parseState, int state);
void pushConditional(ParseState* parseState, const char* name);
void setConditionalOp(ParseState* parseState, ConditionalOp op);
void setConditionalVal(ParseState* parseState, const char* value);
int evalConditional(ParseState* parseState);
int switchConditional(ParseState* parseState);
void setConditional(ParseState* parseState, const char* name);
int popConditional(ParseState* parseState);

char* getCurrentFilename(ParseState* parseState);

ParseState* parseStateCreate();
void parseStateDelete(ParseState* parseState);

#endif
