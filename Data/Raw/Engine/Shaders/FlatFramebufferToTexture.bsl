Technique =
{
	Language = "HLSL11";
	
	Pass =
	{
		DepthRead = false;
		DepthWrite = false;
		
		Target = 
		{
			Blend = true;
			Color = { ONE, ONE, ADD };
			WriteMask = RGB;
		};		
	
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

Technique =
{
	Language = "GLSL";
	
	Pass =
	{
		DepthRead = false;
		DepthWrite = false;
		
		Target = 
		{
			Blend = true;
			Color = { ONE, ONE, ADD };
			WriteMask = RGB;
		};		
		
		Vertex =
		{
			layout(location = 0) in vec2 bs_position;
			layout(location = 1) in vec2 bs_texcoord0;
			
			layout(location = 0) out vec2 texcoord0;
		
			out gl_PerVertex
			{
				vec4 gl_Position;
			};
		
			void main()
			{
				gl_Position = vec4(bs_position, 0, 1);
				texcoord0 = bs_texcoord0;
			}		
		};
		
		Fragment = 
		{
			layout(location = 0) in vec2 texcoord0;
			layout(location = 0) out vec4 fragColor;
		
			layout(binding = 0) uniform Params 
			{
				uvec2 gFramebufferSize;
				uint gSampleCount;
			};		
		
			layout(binding = 1) uniform samplerBuffer gInput;

			uint getLinearAddress(uvec2 coord, uint sampleIndex)
			{
				return (coord.y * gFramebufferSize.x + coord.x) * gSampleCount + sampleIndex;
			}			

			void main()
			{
				vec2 uv = trunc(texcoord0);
				ivec2 pixelPos = ivec2(uv.x, uv.y);
			
				uint sourceIdx = getLinearAddress(pixelPos, gl_SampleID);
				fragColor = texelFetch(gInput, int(sourceIdx));
			}
		};
	};
};