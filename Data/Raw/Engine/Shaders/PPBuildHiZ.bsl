#include "$ENGINE$\PPBase.bslinc"

technique PPBuildHiZ
{
	mixin PPBase;

	code
	{	
		#if MSAA_COUNT <= 1
		SamplerState gDepthSamp;
		Texture2D gDepthTex;
		#else
		Texture2DMS<float, MSAA_COUNT> gDepthTex;
		#endif
		
		float fsmain(VStoFS input) : SV_Target0
		{
			// First pass, resolve MSAA (if needed)
			#if MSAA_COUNT > 1
				float depth = gDepthTex.Load(trunc(input.uv0), 0);
				
				[unroll]
				for(int i = 1; i < MSAA_COUNT; ++i)
					depth = min(depth, gDepthTex.Load(trunc(input.uv0), i));
				
				return depth;
			#else
				float4 depth = gDepthTex.Gather(gDepthSamp, input.uv0);
				return min(min(depth.x, depth.y), min(depth.z, depth.w));
			#endif
		}	
	};
};