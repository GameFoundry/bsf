Shaders			{#shaders}
===============
[TOC]

All shaders in Banshee are written in BSL (Banshee Shading Language). The core of the language is based on HLSL (High Level Shading Language), with various extensions to make development easier. In this manual we will not cover HLSL syntax, nor talk about shaders in general, and will instead focus on the extra functionality added by BSL.

# Basics 

A simple BSL program that renders a mesh all in white looks like this:
~~~~~~~~~~~~~~
technique MyShader
{
	code
	{
		struct VStoFS
		{
			float4 position : SV_Position;
		};
		
		cbuffer PerObject
		{
			float4x4 gMatWorldViewProj;
			float4x4 gMatWorld;
			float4x4 gMatInvWorld;
		}				
		
		struct VertexInput
		{
			float3 position : POSITION;
		};
		
		VStoFS vsmain(VertexInput input)
		{
			VStoFS output;
		
			output.position = mul(gMatWorldViewProj, input.position);			
			return output;
		}

		float4 fsmain(in VStoFS input) : SV_Target0
		{
			return float4(1.0f, 1.0f, 1.0f 1.0f); 
		}	
	};
};
~~~~~~~~~~~~~~

As you can see, aside from the **technique** and **code** blocks, the shader code looks identical to HLSL. Each BSL shader must contain at least one **technique**. Inside the technique you can use a variety of options, but the minimum required is the **code** block, which allows you to specify programmable shader code. Using just this syntax you get the full power of HLSL, as if you were using it directly.

There are a few restrictions compared to normal HLSL that you must be aware of:
 - All non-object shader constants (uniforms in GLSL lingo) must be part of a **cbuffer**. Non-object types are any types that are not textures, buffers or samplers.
 - Entry points for specific shader stages must have specific names. In the example above **vsmain** serves as the entry point for the vertex shader, while **fsmain** serves as the entry point for the fragment (pixel) shader. The full list of entry point names per type:
  - **vsmain** - Vertex shader
  - **gsmain** - Geometry shader
  - **hsmain** - Hull (Tesselation control) shader
  - **dsmain** - Domain (Tesselation evaluation) shader
  - **fsmain** - Fragment (Pixel) shader
  - **csmain** - Compute shader
  
Let's now move onto more advanced functionality specific to BSL.

# Non-programmable states
Aside from the **code** block, a **technique** can also specify four blocks that allow it to control non-programmable parts of the pipeline:
 - **raster** - Allows you to set options related to rasterization, like rasterization mode (fill/wireframe), cull mode, etc.
 - **depth** - Allows you to set options related to depth buffer and depth comparison, like enabling/disabling depth reads or writes, or changing the depth comparison function
 - **stencil** - Allows you to set options related to the stencil buffer and stencil test, like enabling stencil test and setting the test operations
 - **blend** - Allows you to set blending options, like enabling/disabling blending, setting blend operations or setting color the write mask

An example shader using a variety of these blocks is shown:
~~~~~~~~~~~~~~
technique MyShader
{
	// No depth reads of writes
	depth
	{
		read = false;
		write = false;
	};
	
	// Enable blending on the first render target
	blend
	{
		target	
		{
			enabled = true;
			color = { srcA, srcIA, add };
			writemask = RGB;
		};
	};
	
	// Disable culling
	raster
	{
		cull = none;
	};
	
	// Only render if stencil test passes
	stencil
	{
		enabled = true;
		front = { keep, keep, keep, lte };
		reference = 1;
	};

	code
	{
		// ... some shader code
	};
};
~~~~~~~~~~~~~~

Let's cover all available options for each block type. 

## raster

Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
fill    	  	     | wire, solid (See @ref bs::PolygonMode "PolygonMode")			   | @ref bs::RASTERIZER_STATE_DESC::polygonMode "RASTERIZER_STATE_DESC::polygonMode"
cull    	  	     | cw, ccw, none (See @ref bs::CullingMode "CullingMode")			   | @ref bs::RASTERIZER_STATE_DESC::cullMode "RASTERIZER_STATE_DESC::cullMode"
scissor				 | true, false				   | @ref bs::RASTERIZER_STATE_DESC::scissorEnable "RASTERIZER_STATE_DESC::scissorEnable"
multisample			 | true, false				   | @ref bs::RASTERIZER_STATE_DESC::multisampleEnable "RASTERIZER_STATE_DESC::multisampleEnable"
lineaa				 | true, false				   | @ref bs::RASTERIZER_STATE_DESC::antialiasedLineEnable "RASTERIZER_STATE_DESC::antialiasedLineEnable"

## depth
Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
read    	  	     | true, false				   | @ref bs::DEPTH_STENCIL_STATE_DESC::depthReadEnable "DEPTH_STENCIL_STATE_DESC::depthReadEnable"
write    	  	     | true, false				   | @ref bs::DEPTH_STENCIL_STATE_DESC::depthWriteEnable "DEPTH_STENCIL_STATE_DESC::depthWriteEnable"
compare    	  	     | never, always, lt, lte, eq, neq, gte, gt (See @ref bs::CompareFunction "CompareFunction")				   | @ref bs::DEPTH_STENCIL_STATE_DESC::depthComparisonFunc "DEPTH_STENCIL_STATE_DESC::depthComparisonFunc"
bias    	  	     | float				   | @ref bs::RASTERIZER_STATE_DESC::depthBias "RASTERIZER_STATE_DESC::depthBias"
scaledBias    	  	 | float				   | @ref bs::RASTERIZER_STATE_DESC::slopeScaledDepthBias "RASTERIZER_STATE_DESC::slopeScaledDepthBias"
clip    	  	     | true, false				   | @ref bs::RASTERIZER_STATE_DESC::depthClipEnable "RASTERIZER_STATE_DESC::depthClipEnable"

## stencil
Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
reference    	  	 | integer			           | Reference value to use for stencil compare operations.
enabled    	  	     | true, false				   | @ref bs::DEPTH_STENCIL_STATE_DESC::stencilEnable "DEPTH_STENCIL_STATE_DESC::stencilEnable"
readmask    	  	 | R, G, B, A or any combination (e.g. RG, RBA, RGBA). "empty" for zero mask.				   | @ref bs::DEPTH_STENCIL_STATE_DESC::stencilReadMask "DEPTH_STENCIL_STATE_DESC::stencilReadMask"
writemask    	  	 | R, G, B, A or any combination (e.g. RG, RBA, RGBA). "empty" for zero mask.				   | @ref bs::DEPTH_STENCIL_STATE_DESC::stencilWriteMask "DEPTH_STENCIL_STATE_DESC::stencilWriteMask"
front				 | StencilOp block			   | Stencil operations and compare function for front facing geometry
back				 | StencilOp block			   | Stencil operations and compare function for back facing geometry
 
**front** and **back** options are blocks themselves, and they accept the following four options:
Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
fail    	  	 	| keep, zero, replace, inc, dec, incwrap, decwrap, inverse (See @ref bs::StencilOperation "StencilOperation")			           | @ref bs::DEPTH_STENCIL_STATE_DESC::frontStencilFailOp "DEPTH_STENCIL_STATE_DESC::frontStencilFailOp" & @ref bs::DEPTH_STENCIL_STATE_DESC::backStencilFailOp "DEPTH_STENCIL_STATE_DESC::backStencilFailOp"
zfail    	  	 	| keep, zero, replace, inc, dec, incwrap, decwrap, inverse (See @ref bs::StencilOperation "StencilOperation")			           | @ref bs::DEPTH_STENCIL_STATE_DESC::frontStencilZFailOp "DEPTH_STENCIL_STATE_DESC::frontStencilZFailOp" & @ref bs::DEPTH_STENCIL_STATE_DESC::backStencilZFailOp "DEPTH_STENCIL_STATE_DESC::backStencilZFailOp"
pass    	  	 	| keep, zero, replace, inc, dec, incwrap, decwrap, inverse (See @ref bs::StencilOperation "StencilOperation")			           | @ref bs::DEPTH_STENCIL_STATE_DESC::frontStencilPassOp "DEPTH_STENCIL_STATE_DESC::frontStencilPassOp" & @ref bs::DEPTH_STENCIL_STATE_DESC::backStencilPassOp "DEPTH_STENCIL_STATE_DESC::backStencilPassOp"
compare				| never, always, lt, lte, eq, neq, gte, gt (See @ref bs::CompareFunction "CompareFunction") | @ref bs::DEPTH_STENCIL_STATE_DESC::frontStencilComparisonFunc "DEPTH_STENCIL_STATE_DESC::frontStencilComparisonFunc" & @ref bs::DEPTH_STENCIL_STATE_DESC::backStencilComparisonFunc "DEPTH_STENCIL_STATE_DESC::backStencilComparisonFunc"

An example of a stencil block:
~~~~~~~~~~~~~~
technique MyShader
{
	// Only render if stencil test passes
	stencil
	{
		enabled = true;
		front = { 
			fail = keep; // On stencil test fail don't do anything
			zfail = keep; // On depth test fail don't do anything
			pass = inc; // On stencil test pass, increment stencil buffer by one
			compare = always; // Always pass the stencil test
		};
	};
};
~~~~~~~~~~~~~~

**StencilOp** block can also be declared succintly on a single line. Same code as above, written differently:
~~~~~~~~~~~~~~
technique MyShader
{
	// Only render if stencil test passes
	stencil
	{
		enabled = true;
		front = { keep, keep, inc, always }; // Note the order is always: fail op., zfail op., pass op., compare function
	};
};
~~~~~~~~~~~~~~

## blend
Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
dither    	  	 | true, false			           | @ref bs::BLEND_STATE_DESC::alphaToCoverageEnable "BLEND_STATE_DESC::alphaToCoverageEnable"
independant    	 | true, false			           | @ref bs::BLEND_STATE_DESC::independantBlendEnable "BLEND_STATE_DESC::independantBlendEnable"
target			 | Target block					   | Blend operations for a specific render target. Multiple Target blocks can exist under a single blend block.

**Target** block accepts the following options:
Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
index    	  	     | positive integer		       | Index of the render target these options are applied to. If not specified the index is derived from the order in which Target blocks are defined.
enabled				 | true, false				   | @ref bs::RENDER_TARGET_BLEND_STATE_DESC::blendEnable "RENDER_TARGET_BLEND_STATE_DESC::blendEnable"
writemask		     | R, G, B, A or any combination (e.g. RG, RBA, RGBA). "empty" for zero mask.					   | @ref bs::RENDER_TARGET_BLEND_STATE_DESC::renderTargetWriteMask "RENDER_TARGET_BLEND_STATE_DESC::renderTargetWriteMask"
color    	  	     | BlendOp block	       	   | Represents the blend operation to execute on the color channels.
alpha				 | BlendOp block			   | Represents the blend operation to execute on the alpha channel. Only relevant if independant blend is enabled.

**BlendOp** block accepts the following options:
Name                 | Valid values				   | Reference
---------------------|---------------------------- |-----------------------
source    	  	     | one, zero, dstRGB, srcRGB, dstIRGB, srcIRGB, dstA, srcA, dstIA, srcIA (See @ref bs::BlendFactor "BlendFactor")		       | @ref bs::RENDER_TARGET_BLEND_STATE_DESC::srcBlend "RENDER_TARGET_BLEND_STATE_DESC::srcBlend" & @ref bs::RENDER_TARGET_BLEND_STATE_DESC::srcBlendAlpha "RENDER_TARGET_BLEND_STATE_DESC::srcBlendAlpha"
dest    	  	     | one, zero, dstRGB, srcRGB, dstIRGB, srcIRGB, dstA, srcA, dstIA, srcIA (See @ref bs::BlendFactor "BlendFactor")		       | @ref bs::RENDER_TARGET_BLEND_STATE_DESC::dstBlend "RENDER_TARGET_BLEND_STATE_DESC::dstBlend" & @ref bs::RENDER_TARGET_BLEND_STATE_DESC::dstBlendAlpha "RENDER_TARGET_BLEND_STATE_DESC::dstBlendAlpha"
op    	  	     | add, sub, rsub, min, max (See @ref bs::BlendOperation "BlendOperation")		       | @ref bs::RENDER_TARGET_BLEND_STATE_DESC::blendOp "RENDER_TARGET_BLEND_STATE_DESC::blendOp" & @ref bs::RENDER_TARGET_BLEND_STATE_DESC::blendOpAlpha "RENDER_TARGET_BLEND_STATE_DESC::blendOpAlpha"

An example of a **blend** block:
~~~~~~~~~~~~~~
technique MyShader
{
	blend
	{
		target	
		{
			enabled = true;
			color = { 
				source = srcA;
				dest = srcIA;
				op = add;
			};
			writemask = RGB;
		};
	};
};
~~~~~~~~~~~~~~

**BlendOp** block can also be defined succintly on a single line. Same code as above, written differently:
~~~~~~~~~~~~~~
technique MyShader
{
	blend
	{
		target	
		{
			enabled = true;
			color = { srcA, srcIA, add }; // Note the order is always: source, destination, operation
			writemask = RGB;
		};
	};
};
~~~~~~~~~~~~~~

# Mixins
When writing complex shaders is it is often useful to break them up into components. This is where the concept of a **mixin** comes in. Any shader code or programmable states defined in a **mixin** can be included in any **technique**. Syntax within a **mixin** block is identical to syntax in a **technique** block, meaning you can define code and non-programmable blocks as shown above.

~~~~~~~~~~~~~~
// Provides common functionality that might be useful for many different shaders
mixin MyMixin
{
	code
	{
		Texture2D gGBufferAlbedoTex;
		Texture2D gDepthBufferTex;
		
		struct SurfaceData 
		{
			float4 albedo;
			float depth;
		};
		
		SurfaceData getGBufferData(uint2 pixelPos)
		{
			SurfaceData output;
			output.albedo = gGBufferAlbedoTex.Load(int3(pixelPos, 0));
			output.depth = gDepthBufferTex.Load(int3(pixelPos, 0)).r;
		
			return output;
		}			
	};
};
~~~~~~~~~~~~~~

When a technique wishes to use a mixin, simply add it to the technique using the same **mixin** keyword, followed by its name.
~~~~~~~~~~~~~~
technique MyShader
{
	mixin MyMixin;

	code
	{
		// ...
	
		float4 fsmain(in VStoFS input) : SV_Target0
		{
			uint2 pixelPos = ...;
			
			// Technique can now call methods from the mixin
			SurfaceData surfaceData = getGBufferData(pixelPos);
		
			return surfaceData.albedo; 
		}	
	};
};
~~~~~~~~~~~~~~

Often you will want to define mixins in separate files. BSL files are normally stored with the ".bsl" extension, but when writing include files you should use the ".bslinc" extension instead, in order to prevent the system trying to compile the shader code on its own.

In order to include other files in BSL, use the &#35include command. The paths are relative to the working folder, or if you are working in Banshee 3D editor, the paths are relative to your project folder. You can use the special variables $ENGINE$ and $EDITOR$ to access paths to the builtin engine and editor shaders.

~~~~~~~~~~~~~~
// Include the code for accessing Banshee's GBuffer
#include "$ENGINE$/GBufferInput.bslinc"

technique MyShader
{
	mixin GBufferInput;

	// ...
};
~~~~~~~~~~~~~~

## Mixin overrides
Mixins can override each other if another mixin is defined with the same name. The last defined mixin is considered the override and will be used in the technique. Techniques can also reference mixins that haven't been declared yet. 

~~~~~~~~~~~~~~
// This mixin overrides the MyMixin behaviour we defined above
mixin MyMixin
{
	code
	{
		struct SurfaceData 
		{
			float4 albedo;
			float depth;
		};
		
		SurfaceData getGBufferData(uint2 pixelPos)
		{
			SurfaceData output;
			output.albedo = float4(1, 0, 0, 1); // Always return red color
			output.depth = 0.0f;
		
			return output;
		}			
	};
};
~~~~~~~~~~~~~~

This allows you to override existing engine functionality with your own, without having to fully re-write the shaders - simply override the mixin of the part you are interested in. For example overriding the "Surface" mixin (defined in "$ENGINE$/Surface.bslinc") allows your shader to write custom information to the GBuffer. Overriding the "LightingCommon" mixin (define in "$ENGINE$/LightingCommon.bslinc") allows your shader to override how is lighting information calculated.



TODO - Passes
TODO - Default values
TODO - Attributes: internal, color
TODO - Options
TODO - Tags (Maybe)

















## Properties {#bslfx_c_a}

Technique block should therefore always contain a "Language" property. This will ensure the proper technique is used depending on the render API the engine is set to use.
~~~~~~~~~~~~~~
	Language = "HLSL"
~~~~~~~~~~~~~~

Supported values are "HLSL" and "GLSL".

In case you are using a non-standard render, you can also specify a renderer using the "Renderer" property. This will ensure the propert technique is used as you switch between renderers.
~~~~~~~~~~~~~~
	Renderer = "Default"
~~~~~~~~~~~~~~

Supported values are "Any", or "Default". More values could be available if you are using a custom renderer, but otherwise you don't need to set this property.

And finally you may specify an optional set of tags that serve as hints to the renderer when rendering a specific object. You may specify zero or multiple tags:
~~~~~~~~~~~~~~
	Tags = { "Animated", "OtherTag" }
~~~~~~~~~~~~~~

Currently recognized tags by the default renderer are:
 - Skinned - Technique capable of rendering a skin animated mesh
 - Morph -  Technique capable of rendering a blend shape animated mesh
 - SkinnedMorph -  Technique capable of rendering a skin and blend shape animated mesh

When renderer detects an object is using skinned or morph animation (or both), it will prefer to pick a technique with the relevant tag. Unrecognized tags will just be ignored. Renderer can be extended so it supports custom tags.
 
Once the base properties are defined you can start defining code blocks and states.

## Code blocks {#bslfx_c_b}
Code blocks are elements in the technique that will contain HLSL/GLSL (or other supported language) code. There are six supported code blocks:
 - Vertex
 - Fragment
 - Geometry
 - Hull
 - Domain
 - Compute
 
Each corresponding to the programmable GPU pipline stage of the same name. A code block looks like this:
~~~~~~~~~~~~~~
Vertex =
{
	...raw HLSL/GLSL code...
};
~~~~~~~~~~~~~~

Within a code block BSL parsing rules do not work, anything inside them is treated as native GPU program code in the language specified. BSL expects that main entry methods for each programmable stage are named "main" (similar to the OpenGL requirement). You can use any name for non-entry methods.

Aside from the mentioned code blocks you can also use a "Common" code block. All code in the "Common" code block will be available in all other code blocks.


## Passes {#bslfx_c_d}
Each technique can support one or multiple passes. By default you do not have to specify any passes in technique if your shader only requires a single pass. If multiple passes are required use the "Pass" block.

A "Pass" block supports all the code-block and state properties the Technique supports. It also supports an additional "Index" property that accepts an integer and allows you to specify an order in which the passes are executed. It also allows the pass to be uniquely identified if merging passes (see later sections for information about merging). By default index is not needed and pass order of execution is assumed to be sequential.

If you specify code blocks and/or states in both a Technique and a Pass block, then the values under the Technique block will be inherited by all Pass blocks of that technique.

Pass example:
~~~~~~~~~~~~~~
// Technique with two passes
Technique 
{
	Pass
	{
		Index = 0; // Optional, assumed zero since it's first in technique
		
		// Uses same states and code blocks we described for Technique
	}
	
	Pass
	{
		Index = 1; // Optional, assumed one since it's second in technique
		
		// Uses same states and code blocks we described for Technique
	}
};
~~~~~~~~~~~~~~

# Advanced {#bslfx_d}

## Technique inheritance {#bslfx_d_a}
Techniques can inherit code and properties from one another. Simply define one technique as a base technique and give it a unique name:
~~~~~~~~~~~~~~
Technique : base("MyBaseTechnique") =
{
   ...
};
~~~~~~~~~~~~~~

Then inherit the technique as such:
~~~~~~~~~~~~~~
Technique : inherits("MyBaseTechnique") =
{
   ...
};
~~~~~~~~~~~~~~

You can also chain multiple inherited techniques:
~~~~~~~~~~~~~~
Technique : base("MyMoreSpecificBaseTechnique") : inherits("MyBaseTechnique") = 
{
   ...
};
~~~~~~~~~~~~~~

Or inherit from multiple techniques at once:
~~~~~~~~~~~~~~
Technique 
 : inherits("MyBaseTechnique")
 : inherits("MyOtherBaseTechnique") = 
{
   ...
};
~~~~~~~~~~~~~~

Properties of inherited techniques will be combined, with more specific technique overriding base technique's properties. Techniques defined as base will never be instantiated on their own, and will not be available in the shader unless inherited by another technique. Base techniques should always be defined earlier then more specialized techniques.

Only techniques that share the same "Renderer" and "Language" properties can be inherited. This allows you to use the same technique names across multiple renderers and languages. Optionally you may define technique's "Renderer" or "Language" as "Any" in which case the technique will be inheritable from any language, but its name should then be globally unique.

When merging passes within techniques, pass "Index" properties are compared and passes with the same index are merged. If no index is specified, passes are merged sequentially according to their order in the techniques.

Code blocks are merged in the order they are defined.

## Pre-processor {#bslfx_d_b}
Use \#include "Path/To/File.bslinc" to share code by including other BSL files. Included files must end with a .bslinc extension but otherwise their syntax is the same as a normal BSL file. The provided path is a path to the shader relative to the project library if running in editor, or relative to the working directory otherwise. You can also use built-in $ENGINE$ and $EDITOR$ folders to access builtin shaders. e.g.
~~~~~~~~~~~~~~
#include "$ENGINE$/SpriteImage.bslinc"
~~~~~~~~~~~~~~

These values correspond to "Data/Engine/Includes" and "Data/Editor/Includes" folders, respectively. Be aware that $EDITOR$ folder will not be available in your standalone game.

Use standard pre-processor commands \#define/\#undef/\#ifdef/\#ifndef/\#else/\#elif/\#endif to conditionally compile parts of the BSL file.

Macros using \#defines are not supported in BSL code, but are within code-blocks. So while you are allowed to write:
~~~~~~~~~~~~~~
#define PI 3.14
~~~~~~~~~~~~~~

Any references to PI outside of code-blocks will not be replaced with 3.14 and will likely result in an error due to an unrecognized identifier.

## Global shader properties {#bslfx_d_c}
On the top-most level you may also specify additional parameters along with "Parameters", "Blocks" and "Technique":
 - Separable = [true/false]; - Specifies if passes within the shader need to be executed sequentially, or could some other shader be executed in-between them. This is an optimization as it can allow the system to render geometry sharing the same passes all at once. False by default.
 - Sort - [FRONTTOBACK/BACKTOFRONT/NOSORT]; - Specifies in what order are the objects using this shader sorted before rendering.
 - Priority - int; - Specifies when will objects with this shader be rendered compared to other objects. Higher value means the objects will be rendered sooner. Priority has higher importance than sorting.
 - Transparent - [true/false]; - Determines whether the shader renders transparent surfaces. Allows the renderer to better handle the shader.
 
## Sampler state default values {#bslfx_d_d}
Earlier we mentioned that sampler states can be provided a set of default values in a form of their own block, but didn't specify their properties. Sampler state properties are:
 - AddressMode = AddressModeBlock;
 - MinFilter = [NOFILTER/POINT/LINEAR/ANISO/POINTC/LINEARC/ANISOC];
 - MaxFilter = [NOFILTER/POINT/LINEAR/ANISO/POINTC/LINEARC/ANISOC];
 - MipFiler = [NOFILTER/POINT/LINEAR/ANISO/POINTC/LINEARC/ANISOC];
 - MaxAniso = int;
 - MipmapBias = float;
 - MipMin = float;
 - MipMax = float;
 - BorderColor = { float, float, float, float };
 - CompareFunc = [FAIL/PASS/LT/GT/LTE/GTE/EQ/NEQ];
 
Where AddressModeBlock has the following properties:
 - U = [WRAP/MIRROR/CLAMP/BORDER];
 - V = [WRAP/MIRROR/CLAMP/BORDER];
 - W = [WRAP/MIRROR/CLAMP/BORDER];
 
It can also be specified in short form, where parameters are in order as above. For example:
 - AddressMode = { WRAP, WRAP, WRAP };
 
See @ref bs::SamplerState "SamplerState" documentation about the meaning of these properties. 
