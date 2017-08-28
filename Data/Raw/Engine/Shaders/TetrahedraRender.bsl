#include "$ENGINE$\PerCameraData.bslinc"

technique TetrahedraRender
{
	mixin PerCameraData;

	raster
	{
		cull = cw;
	};
	
	depth
	{
		compare = lte;
	};
	
	code
	{
		struct VertexInput
		{
			float3 position : POSITION;
			uint index : TEXCOORD0;			
		};		
	
		struct VStoFS
		{
			float4 position : SV_Position;
			float4 clipPos : TEXCOORD0;
			uint index : TEXCOORD1;
		};
		
		VStoFS vsmain(VertexInput input)
		{
			VStoFS output;
		
			output.position = mul(gMatViewProj, float4(input.position, 1.0f));
			output.clipPos = output.position;
			output.index = input.index;
			
			return output;
		}
		
		#ifndef MSAA
			#define MSAA 0
		#endif
		
		#ifndef MSAA_RESOLVE_0TH
			#define MSAA_RESOLVE_0TH 0
		#endif		

		#if MSAA
		Texture2DMS<float> gDepthBufferTex;
		#else
		Texture2D gDepthBufferTex;
		SamplerState gDepthBufferSamp;
		#endif		
		
		uint fsmain(VStoFS input
		#if MSAA && !MSAA_RESOLVE_0TH
			,uint sampleIdx : SV_SampleIndex
		#endif
		) : SV_Target0
		{
			float sceneDepth;
			#if MSAA
				#if MSAA_RESOLVE_0TH
					sceneDepth = gDepthBufferTex.Load(trunc(input.position.xy), 0);
				#else
					sceneDepth = gDepthBufferTex.Load(trunc(input.position.xy), sampleIdx);
				#endif
			#else
				float2 ndcPos = input.clipPos.xy / input.clipPos.w;
				sceneDepth = gDepthBufferTex.Sample(gDepthBufferSamp, NDCToUV(ndcPos));
			#endif
			
			float currentDepth = input.position.z;
			if(currentDepth < sceneDepth)
				discard;
				
			return input.index;
		}
	};
};