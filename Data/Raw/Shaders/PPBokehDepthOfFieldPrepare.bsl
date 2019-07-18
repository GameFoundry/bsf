#include "$ENGINE$\PPDepthOfFieldCommon.bslinc"
#include "$ENGINE$\PPBase.bslinc"
#include "$ENGINE$\DepthInput.bslinc"

shader PPBokehDepthOfFieldPrepare
{
	mixin PPDepthOfFieldCommon;
	mixin PPBase;
	mixin DepthInput;
	
	code
	{
		cbuffer Params
		{
			float2 gInvInputSize;
		};
		
		Texture2D gInputTex;
		SamplerState gInputSampler 
		{
			Filter = MIN_MAG_MIP_POINT;
		};
	
		float4 fsmain(VStoFS input) : SV_Target0
		{
			float2 uvs[4];
			uvs[0] = input.uv0 + gInvInputSize * float2(-0.5f, -0.5f);
			uvs[1] = input.uv0 + gInvInputSize * float2( 0.5f, -0.5f);
			uvs[2] = input.uv0 + gInvInputSize * float2(-0.5f,  0.5f);
			uvs[3] = input.uv0 + gInvInputSize * float2( 0.5f,  0.5f);

			float4 samples[4];
			float2 layers[4];

			[unroll]
			for(uint i = 0; i < 4; ++i)
			{
				samples[i].rgb = gInputTex.Sample(gInputSampler, uvs[i]).rgb;
				samples[i].a = -convertFromDeviceZ(gDepthBufferTex.SampleLevel(gDepthBufferSamp, uvs[i], 0).r);
				layers[i] = computeLayerContributions(samples[i].a);
			}

			float2 layerSum = layers[0] + layers[1] + layers[2] + layers[3];
			bool isFrontLayer = layerSum.x > layerSum.y;

			float4 mask = isFrontLayer 
				? float4(layers[0].x, layers[1].x, layers[2].x, layers[3].x) 
				: float4(layers[0].y, layers[1].y, layers[2].y, layers[3].y);

			float maskSum = dot(mask, 1);

			[flatten]
			if(maskSum > 0.001f)
				return (samples[0] * mask.x + samples[1] * mask.y + samples[2] * mask.z + samples[3] * mask.w) / maskSum;

			return samples[0];
		}	
	};
};