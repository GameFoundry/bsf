Technique =
{
	Pass =
	{
		DepthRead = false;
		DepthWrite = false;
		
		Common = 
		{
			struct VStoFS
			{
				float4 position : SV_POSITION;
				float2 uv0 : TEXCOORD0;
			};
		};
	
		Vertex =
		{
			struct VertexInput
			{
				float2 screenPos : POSITION;
				float2 uv0 : TEXCOORD0;
			};
			
			VStoFS main(VertexInput input)
			{
				VStoFS output;
			
				output.position = float4(input.screenPos, 0, 1);
				output.uv0 = input.uv0;

				return output;
			}			
		};
		
		Fragment = 
		{
			cbuffer Params : register(b0)
			{
				uint2 gFramebufferSize;
				uint gSampleCount;
			}		
		
			Buffer<float4> gInput : register(t0);

			uint getLinearAddress(uint2 coord, uint sampleIndex)
			{
				return (coord.y * gFramebufferSize.x + coord.x) * gSampleCount + sampleIndex;
			}			

			float4 main(VStoFS input, uint sampleIndex : SV_SampleIndex) : SV_Target0
			{
				int2 pixelPos = trunc(input.uv0);
				uint sourceIdx = getLinearAddress(pixelPos, sampleIndex);

				return gInput[sourceIdx];
			}
		};
	};
};