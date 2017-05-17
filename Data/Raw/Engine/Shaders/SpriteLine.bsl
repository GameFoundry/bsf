technique SpriteLine
{
	blend
	{
		target	
		{
			enabled = true;
			color = { srcA, srcIA, add };
			writemask = RGB;
		};
	};	
	
	depth
	{
		read = false;
		write = false;
	};
	
	raster
	{
		multisample = false; // This controls line rendering algorithm
		lineaa = false;
	};
	
	code
	{
		struct VStoFS
		{
			float4 position : SV_POSITION;
		};

		cbuffer VertParams
		{
			float invViewportWidth;
			float invViewportHeight;
			float4x4 worldTransform;
		};
		
		struct VertexInput
		{
			float2 position : POSITION;
		};			
		
		VStoFS vsmain(VertexInput input)
		{
			float4 tfrmdPos = mul(worldTransform, float4(input.position, 0, 1));
			
			float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);
			float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);

			VStoFS output;
			output.position = float4(tfrmdX, tfrmdY, 0, 1);

			return output;
		}

		cbuffer VertParams
		{
			float4 tint;
		};
		
		float4 fsmain(VStoFS input) : SV_Target
		{
			return tint;
		}
	};
};