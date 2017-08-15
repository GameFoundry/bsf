technique DebugDraw
{
	#ifdef LINE
	raster
	{
		multisample = false; // This controls line rendering algorithm
		lineaa = true;
	};

	blend
	{
		target
		{
			enabled = true;
			color = { srcA, srcIA, add };
		};
	};
	#endif
	
	#ifdef WIRE
	raster
	{
		fill = wire;
	};
	#endif
	
	code
	{
		#ifndef LINE
			#define LINE 0
		#endif
		
		#ifndef WIRE
			#define WIRE 0
		#endif
	
		cbuffer Params
		{
			float4x4 	gMatViewProj;
			float4		gViewDir;
		}
		
		#if LINE || WIRE
		void vsmain(
			in float3 inPos : POSITION,
			in float4 color : COLOR0,
			out float4 oPosition : SV_Position,
			out float4 oColor : COLOR0)
		{
			oPosition = mul(gMatViewProj, float4(inPos.xyz, 1));
			oColor = color;
		}

		float4 fsmain(in float4 inPos : SV_Position, in float4 color : COLOR0) : SV_Target
		{
			return color;
		}
		#else
		void vsmain(
			in float3 inPos : POSITION,
			in float3 inNormal : NORMAL,
			in float4 color : COLOR0,
			out float4 oPosition : SV_Position,
			out float3 oNormal : NORMAL,
			out float4 oColor : COLOR0)
		{
			oPosition = mul(gMatViewProj, float4(inPos.xyz, 1));
			oNormal = inNormal;
			oColor = color;
		}
		float4 fsmain(in float4 inPos : SV_Position, in float3 normal : NORMAL, in float4 color : COLOR0) : SV_Target
		{
			float4 outColor = color * dot(normalize(normal), -gViewDir);
			outColor.a = color.a;
			
			return outColor;
		}
		#endif
	};
};
