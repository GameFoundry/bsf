#include "$ENGINE$\PPBase.bslinc"

shader PPFXAA
{
	mixin PPBase;

	code
	{
		//----------------------------------------------------------------------------------
		// File:        FXAA\media/FXAA.hlsl
		// SDK Version: v1.2 
		// Email:       gameworks@nvidia.com
		// Site:        http://developer.nvidia.com/
		//
		// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
		//
		// Redistribution and use in source and binary forms, with or without
		// modification, are permitted provided that the following conditions
		// are met:
		//  * Redistributions of source code must retain the above copyright
		//    notice, this list of conditions and the following disclaimer.
		//  * Redistributions in binary form must reproduce the above copyright
		//    notice, this list of conditions and the following disclaimer in the
		//    documentation and/or other materials provided with the distribution.
		//  * Neither the name of NVIDIA CORPORATION nor the names of its
		//    contributors may be used to endorse or promote products derived
		//    from this software without specific prior written permission.
		//
		// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
		// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
		// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
		// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
		// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
		// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
		// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
		// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
		// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
		// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
		//
		//----------------------------------------------------------------------------------
		/*============================================================================


							NVIDIA FXAA 3.11 by TIMOTHY LOTTES

		------------------------------------------------------------------------------
								   INTEGRATION CHECKLIST
		------------------------------------------------------------------------------
		(1.)
		In the shader source, setup defines for the desired configuration.
		When providing multiple shaders (for different presets),
		simply setup the defines differently in multiple files.
		Example,

		  #define FXAA_PC 1
		  #define FXAA_HLSL_5 1
		  #define FXAA_QUALITY__PRESET 12

		Or,

		  #define FXAA_360 1
		  
		Or,

		  #define FXAA_PS3 1
		  
		Etc.

		(2.)
		Then include this file,

		  #include "Fxaa3_11.h"

		(3.)
		Then call the FXAA pixel shader from within your desired shader.
		Look at the FXAA Quality FxaaPixelShader() for docs on inputs.
		As for FXAA 3.11 all inputs for all shaders are the same 
		to enable easy porting between platforms.

		  return FxaaPixelShader(...);

		(4.)
		Insure pass prior to FXAA outputs RGBL (see next section).
		Or use,

		  #define FXAA_GREEN_AS_LUMA 1

		(5.)
		Setup engine to provide the following constants
		which are used in the FxaaPixelShader() inputs,

		  FxaaFloat2 fxaaQualityRcpFrame,
		  FxaaFloat fxaaQualitySubpix,
		  FxaaFloat fxaaQualityEdgeThreshold,
		  FxaaFloat fxaaQualityEdgeThresholdMin,

		Look at the FXAA Quality FxaaPixelShader() for docs on inputs.

		(6.)
		Have FXAA vertex shader run as a full screen triangle,
		and output "pos" and "fxaaConsolePosPos" 
		such that inputs in the pixel shader provide,

		  // {xy} = center of pixel
		  FxaaFloat2 pos,

		  // {xy__} = upper left of pixel
		  // {__zw} = lower right of pixel
		  FxaaFloat4 fxaaConsolePosPos,

		(7.)
		Insure the texture sampler(s) used by FXAA are set to bilinear filtering.


		------------------------------------------------------------------------------
							INTEGRATION - RGBL AND COLORSPACE
		------------------------------------------------------------------------------
		FXAA3 requires RGBL as input unless the following is set, 

		  #define FXAA_GREEN_AS_LUMA 1

		In which case the engine uses green in place of luma,
		and requires RGB input is in a non-linear colorspace.

		RGB should be LDR (low dynamic range).
		Specifically do FXAA after tonemapping.

		RGB data as returned by a texture fetch can be non-linear,
		or linear when FXAA_GREEN_AS_LUMA is not set.
		Note an "sRGB format" texture counts as linear,
		because the result of a texture fetch is linear data.
		Regular "RGBA8" textures in the sRGB colorspace are non-linear.

		If FXAA_GREEN_AS_LUMA is not set,
		luma must be stored in the alpha channel prior to running FXAA.
		This luma should be in a perceptual space (could be gamma 2.0).
		Example pass before FXAA where output is gamma 2.0 encoded,

		  color.rgb = ToneMap(color.rgb); // linear color output
		  color.rgb = sqrt(color.rgb);    // gamma 2.0 color output
		  return color;

		To use FXAA,

		  color.rgb = ToneMap(color.rgb);  // linear color output
		  color.rgb = sqrt(color.rgb);     // gamma 2.0 color output
		  color.a = dot(color.rgb, FxaaFloat3(0.299, 0.587, 0.114)); // compute luma
		  return color;

		Another example where output is linear encoded,
		say for instance writing to an sRGB formated render target,
		where the render target does the conversion back to sRGB after blending,

		  color.rgb = ToneMap(color.rgb); // linear color output
		  return color;

		To use FXAA,

		  color.rgb = ToneMap(color.rgb); // linear color output
		  color.a = sqrt(dot(color.rgb, FxaaFloat3(0.299, 0.587, 0.114))); // compute luma
		  return color;

		Getting luma correct is required for the algorithm to work correctly.


		------------------------------------------------------------------------------
								  BEING LINEARLY CORRECT?
		------------------------------------------------------------------------------
		Applying FXAA to a framebuffer with linear RGB color will look worse.
		This is very counter intuitive, but happends to be true in this case.
		The reason is because dithering artifacts will be more visiable 
		in a linear colorspace.


		------------------------------------------------------------------------------
									 COMPLEX INTEGRATION
		------------------------------------------------------------------------------
		Q. What if the engine is blending into RGB before wanting to run FXAA?

		A. In the last opaque pass prior to FXAA,
		   have the pass write out luma into alpha.
		   Then blend into RGB only.
		   FXAA should be able to run ok
		   assuming the blending pass did not any add aliasing.
		   This should be the common case for particles and common blending passes.

		A. Or use FXAA_GREEN_AS_LUMA.

		============================================================================*/

		/*============================================================================

									 INTEGRATION KNOBS

		============================================================================*/
		#ifndef FXAA_GREEN_AS_LUMA
			//
			// For those using non-linear color,
			// and either not able to get luma in alpha, or not wanting to,
			// this enables FXAA to run using green as a proxy for luma.
			// So with this enabled, no need to pack luma in alpha.
			//
			// This will turn off AA on anything which lacks some amount of green.
			// Pure red and blue or combination of only R and B, will get no AA.
			//
			// Might want to lower the settings for both,
			//    fxaaConsoleEdgeThresholdMin
			//    fxaaQualityEdgeThresholdMin
			// In order to insure AA does not get turned off on colors 
			// which contain a minor amount of green.
			//
			// 1 = On.
			// 0 = Off.
			//
			#define FXAA_GREEN_AS_LUMA 1
		#endif
		/*--------------------------------------------------------------------------*/
		#ifndef FXAA_DISCARD
			//
			// Only valid for PC OpenGL currently.
			// Probably will not work when FXAA_GREEN_AS_LUMA = 1.
			//
			// 1 = Use discard on pixels which don't need AA.
			//     For APIs which enable concurrent TEX+ROP from same surface.
			// 0 = Return unchanged color on pixels which don't need AA.
			//
			#define FXAA_DISCARD 0
		#endif
		/*--------------------------------------------------------------------------*/
		#ifndef FXAA_GATHER4_ALPHA
			#define FXAA_GATHER4_ALPHA 1
		#endif

		/*============================================================================
								FXAA QUALITY - TUNING KNOBS
		------------------------------------------------------------------------------
		NOTE the other tuning knobs are now in the shader function inputs!
		============================================================================*/
		#ifndef FXAA_QUALITY__PRESET
			//
			// Choose the quality preset.
			// This needs to be compiled into the shader as it effects code.
			// Best option to include multiple presets is to 
			// in each shader define the preset, then include this file.
			// 
			// OPTIONS
			// -----------------------------------------------------------------------
			// 10 to 15 - default medium dither (10=fastest, 15=highest quality)
			// 20 to 29 - less dither, more expensive (20=fastest, 29=highest quality)
			// 39       - no dither, very expensive 
			//
			// NOTES
			// -----------------------------------------------------------------------
			// 12 = slightly faster then FXAA 3.9 and higher edge quality (default)
			// 13 = about same speed as FXAA 3.9 and better than 12
			// 23 = closest to FXAA 3.9 visually and performance wise
			//  _ = the lowest digit is directly related to performance
			// _  = the highest digit is directly related to style
			// 
			#define FXAA_QUALITY__PRESET 12
		#endif


		/*============================================================================

								   FXAA QUALITY - PRESETS

		============================================================================*/

		/*============================================================================
							 FXAA QUALITY - MEDIUM DITHER PRESETS
		============================================================================*/
		#if (FXAA_QUALITY__PRESET == 10)
			#define FXAA_QUALITY__PS 3
			#define FXAA_QUALITY__P0 1.5
			#define FXAA_QUALITY__P1 3.0
			#define FXAA_QUALITY__P2 12.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 11)
			#define FXAA_QUALITY__PS 4
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 3.0
			#define FXAA_QUALITY__P3 12.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 12)
			#define FXAA_QUALITY__PS 5
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 4.0
			#define FXAA_QUALITY__P4 12.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 13)
			#define FXAA_QUALITY__PS 6
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 4.0
			#define FXAA_QUALITY__P5 12.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 14)
			#define FXAA_QUALITY__PS 7
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 4.0
			#define FXAA_QUALITY__P6 12.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 15)
			#define FXAA_QUALITY__PS 8
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 2.0
			#define FXAA_QUALITY__P6 4.0
			#define FXAA_QUALITY__P7 12.0
		#endif

		/*============================================================================
							 FXAA QUALITY - LOW DITHER PRESETS
		============================================================================*/
		#if (FXAA_QUALITY__PRESET == 20)
			#define FXAA_QUALITY__PS 3
			#define FXAA_QUALITY__P0 1.5
			#define FXAA_QUALITY__P1 2.0
			#define FXAA_QUALITY__P2 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 21)
			#define FXAA_QUALITY__PS 4
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 22)
			#define FXAA_QUALITY__PS 5
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 23)
			#define FXAA_QUALITY__PS 6
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 24)
			#define FXAA_QUALITY__PS 7
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 3.0
			#define FXAA_QUALITY__P6 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 25)
			#define FXAA_QUALITY__PS 8
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 2.0
			#define FXAA_QUALITY__P6 4.0
			#define FXAA_QUALITY__P7 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 26)
			#define FXAA_QUALITY__PS 9
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 2.0
			#define FXAA_QUALITY__P6 2.0
			#define FXAA_QUALITY__P7 4.0
			#define FXAA_QUALITY__P8 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 27)
			#define FXAA_QUALITY__PS 10
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 2.0
			#define FXAA_QUALITY__P6 2.0
			#define FXAA_QUALITY__P7 2.0
			#define FXAA_QUALITY__P8 4.0
			#define FXAA_QUALITY__P9 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 28)
			#define FXAA_QUALITY__PS 11
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 2.0
			#define FXAA_QUALITY__P6 2.0
			#define FXAA_QUALITY__P7 2.0
			#define FXAA_QUALITY__P8 2.0
			#define FXAA_QUALITY__P9 4.0
			#define FXAA_QUALITY__P10 8.0
		#endif
		/*--------------------------------------------------------------------------*/
		#if (FXAA_QUALITY__PRESET == 29)
			#define FXAA_QUALITY__PS 12
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.5
			#define FXAA_QUALITY__P2 2.0
			#define FXAA_QUALITY__P3 2.0
			#define FXAA_QUALITY__P4 2.0
			#define FXAA_QUALITY__P5 2.0
			#define FXAA_QUALITY__P6 2.0
			#define FXAA_QUALITY__P7 2.0
			#define FXAA_QUALITY__P8 2.0
			#define FXAA_QUALITY__P9 2.0
			#define FXAA_QUALITY__P10 4.0
			#define FXAA_QUALITY__P11 8.0
		#endif

		/*============================================================================
							 FXAA QUALITY - EXTREME QUALITY
		============================================================================*/
		#if (FXAA_QUALITY__PRESET == 39)
			#define FXAA_QUALITY__PS 12
			#define FXAA_QUALITY__P0 1.0
			#define FXAA_QUALITY__P1 1.0
			#define FXAA_QUALITY__P2 1.0
			#define FXAA_QUALITY__P3 1.0
			#define FXAA_QUALITY__P4 1.0
			#define FXAA_QUALITY__P5 1.5
			#define FXAA_QUALITY__P6 2.0
			#define FXAA_QUALITY__P7 2.0
			#define FXAA_QUALITY__P8 2.0
			#define FXAA_QUALITY__P9 2.0
			#define FXAA_QUALITY__P10 4.0
			#define FXAA_QUALITY__P11 8.0
		#endif
		
		SamplerState gInputSamp;
		Texture2D gInputTex;

		/*============================================================================

										API PORTING

		============================================================================*/
		#define FxaaBool bool
		#define FxaaDiscard clip(-1)
		#define FxaaFloat float
		#define FxaaFloat2 float2
		#define FxaaFloat3 float3
		#define FxaaFloat4 float4
		#define FxaaHalf half
		#define FxaaHalf2 half2
		#define FxaaHalf3 half3
		#define FxaaHalf4 half4
		#define FxaaSat(x) saturate(x)

		#define FxaaInt2 int2
		#define FxaaTexTop(p) gInputTex.SampleLevel(gInputSamp, p, 0.0)
		#define FxaaTexOff(p, o, r) gInputTex.SampleLevel(gInputSamp, p, 0.0, o)
		#define FxaaTexAlpha4(p) gInputTex.GatherAlpha(gInputSamp, p)
		#define FxaaTexOffAlpha4(p, o) gInputTex.GatherAlpha(gInputSamp, p, o)
		#define FxaaTexGreen4(p) gInputTex.GatherGreen(gInputSamp, p)
		#define FxaaTexOffGreen4(p, o) gInputTex.GatherGreen(gInputSamp, p, o)


		/*============================================================================
						   GREEN AS LUMA OPTION SUPPORT FUNCTION
		============================================================================*/
		#if (FXAA_GREEN_AS_LUMA == 0)
			FxaaFloat FxaaLuma(FxaaFloat4 rgba) { return rgba.w; }
		#else
			FxaaFloat FxaaLuma(FxaaFloat4 rgba) { return rgba.y; }
		#endif    

		/*============================================================================

									 FXAA3 QUALITY - PC

		============================================================================*/
		FxaaFloat4 FxaaPixelShader(
			//
			// Use noperspective interpolation here (turn off perspective interpolation).
			// {xy} = center of pixel
			FxaaFloat2 pos,
			//
			// Only used on FXAA Quality.
			// This must be from a constant/uniform.
			// {x_} = 1.0/screenWidthInPixels
			// {_y} = 1.0/screenHeightInPixels
			FxaaFloat2 fxaaQualityRcpFrame,
			//
			// Only used on FXAA Quality.
			// This used to be the FXAA_QUALITY__SUBPIX define.
			// It is here now to allow easier tuning.
			// Choose the amount of sub-pixel aliasing removal.
			// This can effect sharpness.
			//   1.00 - upper limit (softer)
			//   0.75 - default amount of filtering
			//   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
			//   0.25 - almost off
			//   0.00 - completely off
			FxaaFloat fxaaQualitySubpix,
			//
			// Only used on FXAA Quality.
			// This used to be the FXAA_QUALITY__EDGE_THRESHOLD define.
			// It is here now to allow easier tuning.
			// The minimum amount of local contrast required to apply algorithm.
			//   0.333 - too little (faster)
			//   0.250 - low quality
			//   0.166 - default
			//   0.125 - high quality 
			//   0.063 - overkill (slower)
			FxaaFloat fxaaQualityEdgeThreshold,
			//
			// Only used on FXAA Quality.
			// This used to be the FXAA_QUALITY__EDGE_THRESHOLD_MIN define.
			// It is here now to allow easier tuning.
			// Trims the algorithm from processing darks.
			//   0.0833 - upper limit (default, the start of visible unfiltered edges)
			//   0.0625 - high quality (faster)
			//   0.0312 - visible limit (slower)
			// Special notes when using FXAA_GREEN_AS_LUMA,
			//   Likely want to set this to zero.
			//   As colors that are mostly not-green
			//   will appear very dark in the green channel!
			//   Tune by looking at mostly non-green content,
			//   then start at zero and increase until aliasing is a problem.
			FxaaFloat fxaaQualityEdgeThresholdMin
		) {
		/*--------------------------------------------------------------------------*/
			FxaaFloat2 posM;
			posM.x = pos.x;
			posM.y = pos.y;
			#if (FXAA_GATHER4_ALPHA == 1)
				#if (FXAA_DISCARD == 0)
					FxaaFloat4 rgbyM = FxaaTexTop(posM);
					#if (FXAA_GREEN_AS_LUMA == 0)
						#define lumaM rgbyM.w
					#else
						#define lumaM rgbyM.y
					#endif
				#endif
				#if (FXAA_GREEN_AS_LUMA == 0)
					FxaaFloat4 luma4A = FxaaTexAlpha4(posM);
					FxaaFloat4 luma4B = FxaaTexOffAlpha4(posM, FxaaInt2(-1, -1));
				#else
					FxaaFloat4 luma4A = FxaaTexGreen4(posM);
					FxaaFloat4 luma4B = FxaaTexOffGreen4(posM, FxaaInt2(-1, -1));
				#endif
				#if (FXAA_DISCARD == 1)
					#define lumaM luma4A.w
				#endif
				#define lumaE luma4A.z
				#define lumaS luma4A.x
				#define lumaSE luma4A.y
				#define lumaNW luma4B.w
				#define lumaN luma4B.z
				#define lumaW luma4B.x
			#else
				FxaaFloat4 rgbyM = FxaaTexTop(posM);
				#if (FXAA_GREEN_AS_LUMA == 0)
					#define lumaM rgbyM.w
				#else
					#define lumaM rgbyM.y
				#endif
				FxaaFloat lumaS = FxaaLuma(FxaaTexOff(posM, FxaaInt2( 0, 1), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaE = FxaaLuma(FxaaTexOff(posM, FxaaInt2( 1, 0), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaN = FxaaLuma(FxaaTexOff(posM, FxaaInt2( 0,-1), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaW = FxaaLuma(FxaaTexOff(posM, FxaaInt2(-1, 0), fxaaQualityRcpFrame.xy));
			#endif
		/*--------------------------------------------------------------------------*/
			FxaaFloat maxSM = max(lumaS, lumaM);
			FxaaFloat minSM = min(lumaS, lumaM);
			FxaaFloat maxESM = max(lumaE, maxSM);
			FxaaFloat minESM = min(lumaE, minSM);
			FxaaFloat maxWN = max(lumaN, lumaW);
			FxaaFloat minWN = min(lumaN, lumaW);
			FxaaFloat rangeMax = max(maxWN, maxESM);
			FxaaFloat rangeMin = min(minWN, minESM);
			FxaaFloat rangeMaxScaled = rangeMax * fxaaQualityEdgeThreshold;
			FxaaFloat range = rangeMax - rangeMin;
			FxaaFloat rangeMaxClamped = max(fxaaQualityEdgeThresholdMin, rangeMaxScaled);
			FxaaBool earlyExit = range < rangeMaxClamped;
		/*--------------------------------------------------------------------------*/
			if(earlyExit)
				#if (FXAA_DISCARD == 1)
					FxaaDiscard;
				#else
					return rgbyM;
				#endif
		/*--------------------------------------------------------------------------*/
			#if (FXAA_GATHER4_ALPHA == 0)
				FxaaFloat lumaNW = FxaaLuma(FxaaTexOff(posM, FxaaInt2(-1,-1), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaSE = FxaaLuma(FxaaTexOff(posM, FxaaInt2( 1, 1), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaNE = FxaaLuma(FxaaTexOff(posM, FxaaInt2( 1,-1), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaSW = FxaaLuma(FxaaTexOff(posM, FxaaInt2(-1, 1), fxaaQualityRcpFrame.xy));
			#else
				FxaaFloat lumaNE = FxaaLuma(FxaaTexOff(posM, FxaaInt2(1, -1), fxaaQualityRcpFrame.xy));
				FxaaFloat lumaSW = FxaaLuma(FxaaTexOff(posM, FxaaInt2(-1, 1), fxaaQualityRcpFrame.xy));
			#endif
		/*--------------------------------------------------------------------------*/
			FxaaFloat lumaNS = lumaN + lumaS;
			FxaaFloat lumaWE = lumaW + lumaE;
			FxaaFloat subpixRcpRange = 1.0/range;
			FxaaFloat subpixNSWE = lumaNS + lumaWE;
			FxaaFloat edgeHorz1 = (-2.0 * lumaM) + lumaNS;
			FxaaFloat edgeVert1 = (-2.0 * lumaM) + lumaWE;
		/*--------------------------------------------------------------------------*/
			FxaaFloat lumaNESE = lumaNE + lumaSE;
			FxaaFloat lumaNWNE = lumaNW + lumaNE;
			FxaaFloat edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
			FxaaFloat edgeVert2 = (-2.0 * lumaN) + lumaNWNE;
		/*--------------------------------------------------------------------------*/
			FxaaFloat lumaNWSW = lumaNW + lumaSW;
			FxaaFloat lumaSWSE = lumaSW + lumaSE;
			FxaaFloat edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
			FxaaFloat edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
			FxaaFloat edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
			FxaaFloat edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
			FxaaFloat edgeHorz = abs(edgeHorz3) + edgeHorz4;
			FxaaFloat edgeVert = abs(edgeVert3) + edgeVert4;
		/*--------------------------------------------------------------------------*/
			FxaaFloat subpixNWSWNESE = lumaNWSW + lumaNESE;
			FxaaFloat lengthSign = fxaaQualityRcpFrame.x;
			FxaaBool horzSpan = edgeHorz >= edgeVert;
			FxaaFloat subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;
		/*--------------------------------------------------------------------------*/
			if(!horzSpan) lumaN = lumaW;
			if(!horzSpan) lumaS = lumaE;
			if(horzSpan) lengthSign = fxaaQualityRcpFrame.y;
			FxaaFloat subpixB = (subpixA * (1.0/12.0)) - lumaM;
		/*--------------------------------------------------------------------------*/
			FxaaFloat gradientN = lumaN - lumaM;
			FxaaFloat gradientS = lumaS - lumaM;
			FxaaFloat lumaNN = lumaN + lumaM;
			FxaaFloat lumaSS = lumaS + lumaM;
			FxaaBool pairN = abs(gradientN) >= abs(gradientS);
			FxaaFloat gradient = max(abs(gradientN), abs(gradientS));
			if(pairN) lengthSign = -lengthSign;
			FxaaFloat subpixC = FxaaSat(abs(subpixB) * subpixRcpRange);
		/*--------------------------------------------------------------------------*/
			FxaaFloat2 posB;
			posB.x = posM.x;
			posB.y = posM.y;
			FxaaFloat2 offNP;
			offNP.x = (!horzSpan) ? 0.0 : fxaaQualityRcpFrame.x;
			offNP.y = ( horzSpan) ? 0.0 : fxaaQualityRcpFrame.y;
			if(!horzSpan) posB.x += lengthSign * 0.5;
			if( horzSpan) posB.y += lengthSign * 0.5;
		/*--------------------------------------------------------------------------*/
			FxaaFloat2 posN;
			posN.x = posB.x - offNP.x * FXAA_QUALITY__P0;
			posN.y = posB.y - offNP.y * FXAA_QUALITY__P0;
			FxaaFloat2 posP;
			posP.x = posB.x + offNP.x * FXAA_QUALITY__P0;
			posP.y = posB.y + offNP.y * FXAA_QUALITY__P0;
			FxaaFloat subpixD = ((-2.0)*subpixC) + 3.0;
			FxaaFloat lumaEndN = FxaaLuma(FxaaTexTop(posN));
			FxaaFloat subpixE = subpixC * subpixC;
			FxaaFloat lumaEndP = FxaaLuma(FxaaTexTop(posP));
		/*--------------------------------------------------------------------------*/
			if(!pairN) lumaNN = lumaSS;
			FxaaFloat gradientScaled = gradient * 1.0/4.0;
			FxaaFloat lumaMM = lumaM - lumaNN * 0.5;
			FxaaFloat subpixF = subpixD * subpixE;
			FxaaBool lumaMLTZero = lumaMM < 0.0;
		/*--------------------------------------------------------------------------*/
			lumaEndN -= lumaNN * 0.5;
			lumaEndP -= lumaNN * 0.5;
			FxaaBool doneN = abs(lumaEndN) >= gradientScaled;
			FxaaBool doneP = abs(lumaEndP) >= gradientScaled;
			if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P1;
			if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P1;
			FxaaBool doneNP = (!doneN) || (!doneP);
			if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P1;
			if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P1;
		/*--------------------------------------------------------------------------*/
			if(doneNP) {
				if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
				if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
				if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
				if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
				doneN = abs(lumaEndN) >= gradientScaled;
				doneP = abs(lumaEndP) >= gradientScaled;
				if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P2;
				if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P2;
				doneNP = (!doneN) || (!doneP);
				if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P2;
				if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P2;
		/*--------------------------------------------------------------------------*/
				#if (FXAA_QUALITY__PS > 3)
				if(doneNP) {
					if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
					if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
					if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
					if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
					doneN = abs(lumaEndN) >= gradientScaled;
					doneP = abs(lumaEndP) >= gradientScaled;
					if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P3;
					if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P3;
					doneNP = (!doneN) || (!doneP);
					if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P3;
					if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P3;
		/*--------------------------------------------------------------------------*/
					#if (FXAA_QUALITY__PS > 4)
					if(doneNP) {
						if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
						if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
						if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
						if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
						doneN = abs(lumaEndN) >= gradientScaled;
						doneP = abs(lumaEndP) >= gradientScaled;
						if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P4;
						if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P4;
						doneNP = (!doneN) || (!doneP);
						if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P4;
						if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P4;
		/*--------------------------------------------------------------------------*/
						#if (FXAA_QUALITY__PS > 5)
						if(doneNP) {
							if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
							if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
							if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
							if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
							doneN = abs(lumaEndN) >= gradientScaled;
							doneP = abs(lumaEndP) >= gradientScaled;
							if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P5;
							if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P5;
							doneNP = (!doneN) || (!doneP);
							if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P5;
							if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P5;
		/*--------------------------------------------------------------------------*/
							#if (FXAA_QUALITY__PS > 6)
							if(doneNP) {
								if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
								if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
								if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
								if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
								doneN = abs(lumaEndN) >= gradientScaled;
								doneP = abs(lumaEndP) >= gradientScaled;
								if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P6;
								if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P6;
								doneNP = (!doneN) || (!doneP);
								if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P6;
								if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P6;
		/*--------------------------------------------------------------------------*/
								#if (FXAA_QUALITY__PS > 7)
								if(doneNP) {
									if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
									if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
									if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
									if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
									doneN = abs(lumaEndN) >= gradientScaled;
									doneP = abs(lumaEndP) >= gradientScaled;
									if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P7;
									if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P7;
									doneNP = (!doneN) || (!doneP);
									if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P7;
									if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P7;
		/*--------------------------------------------------------------------------*/
			#if (FXAA_QUALITY__PS > 8)
			if(doneNP) {
				if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
				if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
				if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
				if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
				doneN = abs(lumaEndN) >= gradientScaled;
				doneP = abs(lumaEndP) >= gradientScaled;
				if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P8;
				if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P8;
				doneNP = (!doneN) || (!doneP);
				if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P8;
				if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P8;
		/*--------------------------------------------------------------------------*/
				#if (FXAA_QUALITY__PS > 9)
				if(doneNP) {
					if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
					if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
					if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
					if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
					doneN = abs(lumaEndN) >= gradientScaled;
					doneP = abs(lumaEndP) >= gradientScaled;
					if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P9;
					if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P9;
					doneNP = (!doneN) || (!doneP);
					if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P9;
					if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P9;
		/*--------------------------------------------------------------------------*/
					#if (FXAA_QUALITY__PS > 10)
					if(doneNP) {
						if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
						if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
						if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
						if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
						doneN = abs(lumaEndN) >= gradientScaled;
						doneP = abs(lumaEndP) >= gradientScaled;
						if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P10;
						if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P10;
						doneNP = (!doneN) || (!doneP);
						if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P10;
						if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P10;
		/*--------------------------------------------------------------------------*/
						#if (FXAA_QUALITY__PS > 11)
						if(doneNP) {
							if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
							if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
							if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
							if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
							doneN = abs(lumaEndN) >= gradientScaled;
							doneP = abs(lumaEndP) >= gradientScaled;
							if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P11;
							if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P11;
							doneNP = (!doneN) || (!doneP);
							if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P11;
							if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P11;
		/*--------------------------------------------------------------------------*/
							#if (FXAA_QUALITY__PS > 12)
							if(doneNP) {
								if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(posN.xy));
								if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(posP.xy));
								if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
								if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
								doneN = abs(lumaEndN) >= gradientScaled;
								doneP = abs(lumaEndP) >= gradientScaled;
								if(!doneN) posN.x -= offNP.x * FXAA_QUALITY__P12;
								if(!doneN) posN.y -= offNP.y * FXAA_QUALITY__P12;
								doneNP = (!doneN) || (!doneP);
								if(!doneP) posP.x += offNP.x * FXAA_QUALITY__P12;
								if(!doneP) posP.y += offNP.y * FXAA_QUALITY__P12;
		/*--------------------------------------------------------------------------*/
							}
							#endif
		/*--------------------------------------------------------------------------*/
						}
						#endif
		/*--------------------------------------------------------------------------*/
					}
					#endif
		/*--------------------------------------------------------------------------*/
				}
				#endif
		/*--------------------------------------------------------------------------*/
			}
			#endif
		/*--------------------------------------------------------------------------*/
								}
								#endif
		/*--------------------------------------------------------------------------*/
							}
							#endif
		/*--------------------------------------------------------------------------*/
						}
						#endif
		/*--------------------------------------------------------------------------*/
					}
					#endif
		/*--------------------------------------------------------------------------*/
				}
				#endif
		/*--------------------------------------------------------------------------*/
			}
		/*--------------------------------------------------------------------------*/
			FxaaFloat dstN = posM.x - posN.x;
			FxaaFloat dstP = posP.x - posM.x;
			if(!horzSpan) dstN = posM.y - posN.y;
			if(!horzSpan) dstP = posP.y - posM.y;
		/*--------------------------------------------------------------------------*/
			FxaaBool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
			FxaaFloat spanLength = (dstP + dstN);
			FxaaBool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
			FxaaFloat spanLengthRcp = 1.0/spanLength;
		/*--------------------------------------------------------------------------*/
			FxaaBool directionN = dstN < dstP;
			FxaaFloat dst = min(dstN, dstP);
			FxaaBool goodSpan = directionN ? goodSpanN : goodSpanP;
			FxaaFloat subpixG = subpixF * subpixF;
			FxaaFloat pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
			FxaaFloat subpixH = subpixG * fxaaQualitySubpix;
		/*--------------------------------------------------------------------------*/
			FxaaFloat pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
			FxaaFloat pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
			if(!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
			if( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;
			#if (FXAA_DISCARD == 1)
				return FxaaTexTop(posM);
			#else
				return FxaaFloat4(FxaaTexTop(posM).xyz, lumaM);
			#endif
		}
		/*==========================================================================*/

		[internal]
		cbuffer Input
		{
			float2 gInvTexSize;
		}		
		
		float4 fsmain(VStoFS input) : SV_Target0
		{
			return FxaaPixelShader(input.uv0, gInvTexSize, 0.75f, 0.166f, 0.0833f);
		}	
	};
};
