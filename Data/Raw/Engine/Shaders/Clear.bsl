technique Clear
{
	depth
	{
		read = false;
		write = false;
	};

	code
	{
		struct VStoFS
		{
			float4 position : SV_Position;
		};

		struct VertexInput
		{
			float2 screenPos : POSITION;
			float2 uv0 : TEXCOORD0;
		};
		
		cbuffer Params
		{
			uint gClearValue;
		};
		
		VStoFS vsmain(VertexInput input)
		{
			VStoFS output;
			output.position = float4(input.screenPos, 0, 1);

			return output;
		}			

		uint4 fsmain(VStoFS input) : SV_Target0
		{
			return uint4(gClearValue, gClearValue, gClearValue, gClearValue);
		}
	};
};