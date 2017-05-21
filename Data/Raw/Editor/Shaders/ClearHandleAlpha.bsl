technique ClearHandleAlpha
{
	blend
	{
		target
		{
			writemask = A;
		};
	};
	
	depth
	{
		read = false;
		write = false;
	};
	
	stencil
	{
		enabled = true;
		front = { keep, keep, keep, lte };
		reference = 1;
	};
	
	code
	{
		float4 vsmain(float2 screenPos : POSITION) : SV_POSITION
		{
			return float4(screenPos, 0, 1);
		}

		float4 fsmain(in float4 inPos : SV_Position) : SV_Target
		{
			return float4(0, 0, 0, 1);
		}	
	};	
};