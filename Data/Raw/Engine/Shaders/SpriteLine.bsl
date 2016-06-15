Parameters =
{
	mat4x4 	worldTransform;
	float	invViewportWidth;
	float	invViewportHeight;
	
	StructBuffer  	linePoints;
	float 			lineWidth;
	float4			tint;
};

Technique =
{
	Language = "HLSL11";
	
	Pass =
	{
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
			WriteMask = RGB;
		};
		
		DepthRead = false;
		DepthWrite = false;
		
		Common = 
		{
			struct VStoFS
			{
				float4 position : SV_POSITION;
				float2 screenPos : TEXCOORD1;
				uint lineIdx : LINEIDX;
			};
		};
		
		Vertex =
		{
			float invViewportWidth;
			float invViewportHeight;
			float4x4 worldTransform;

			struct VertexInput
			{
				float2 position : POSITION;
				uint lineIdx : BLENDINDICES0;
			};			
			
			VStoFS main(VertexInput input)
			{
				float4 tfrmdPos = mul(worldTransform, float4(input.position, 0, 1));
				
				float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);
				float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);

				VStoFS output;
				output.position = float4(tfrmdX, tfrmdY, 0, 1);
				output.screenPos = tfrmdPos.xy;
				output.lineIdx = input.lineIdx;
				
				return output;
			}
		};
		
		Fragment =
		{
			StructuredBuffer<float2> linePoints;
			float lineWidth;
			float4 tint;
			
			float getSquaredDistanceToLine(float2 a, float2 b, float2 pt)
			{
				float2 lineDiff = b - a;
				float sqrdLineLength = dot(lineDiff, lineDiff);
				
				float2 pointDiff = pt - a;
				if(sqrdLineLength < 0.0001f) // a == b
					return dot(pointDiff, pointDiff);
				else
				{
					float t = clamp(dot(pointDiff, lineDiff / sqrdLineLength), 0, 1);
					float2 projPoint = a + lineDiff * t;
					float2 toLineDiff = projPoint - pt;
					return dot(toLineDiff, toLineDiff);
				} 
			}

			float4 main(VStoFS input) : SV_Target
			{
				// Get number of lines
				uint numPoints;
				uint dummy;
				linePoints.GetDimensions(numPoints, dummy);
				
				uint numLines = numPoints - 1;
								
				// Find nearest line
				//// Distance to current line
				int lineIdx = (int)input.lineIdx;
				float2 a = linePoints.Load(lineIdx + 0);				
				float2 b = linePoints.Load(lineIdx + 1);
				
				float sqrdDist = getSquaredDistanceToLine(a, b, input.screenPos);

				//// Distance to previous line
				int prevLineIdx = max(0, lineIdx - 1);
				a = linePoints.Load(prevLineIdx + 0);				
				b = linePoints.Load(prevLineIdx + 1);
				
				sqrdDist = min(sqrdDist, getSquaredDistanceToLine(a, b, input.screenPos));
				
				//// Distance to next line
				int nextLineIdx = min((int)numLines - 1, lineIdx + 1);
				a = linePoints.Load(nextLineIdx + 0);				
				b = linePoints.Load(nextLineIdx + 1);
				
				sqrdDist = min(sqrdDist, getSquaredDistanceToLine(a, b, input.screenPos));
				
				float sqrdLineWidth = lineWidth * lineWidth;
				sqrdDist = max(sqrdDist - sqrdLineWidth, 0.0f);
				
				float featherWidth = 1.0f; // When changing this change the width of the line quads as well
				float filtered = sqrdDist / (featherWidth * featherWidth); // TODO - Use a different filter like Gaussian
				float weight = 1.0f - clamp(filtered, 0, 1);
				return float4(tint.rgb, tint.a * weight);
			}
		};
	};
};

Technique =
{
	Language = "GLSL";
	
	Pass =
	{
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
			WriteMask = RGB;
		};
		
		DepthRead = false;
		DepthWrite = false;
		
		Vertex =
		{
			uniform float invViewportWidth;
			uniform float invViewportHeight;
			uniform mat4 worldTransform;

			in vec2 bs_position;
			in int bs_blendindices;
			
			out vec2 screenPos;
			out flat int lineIdx;

			out gl_PerVertex
			{
				vec4 gl_Position;
			};			
			
			void main()
			{
				vec4 tfrmdPos = worldTransform * vec4(bs_position, 0, 1);
				
				float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);
				float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);

				gl_Position = vec4(tfrmdX, tfrmdY, 0, 1);
				screenPos = tfrmdPos.xy;
				lineIdx = bs_blendindices;
			}
		};
		
		Fragment =
		{
			uniform samplerBuffer linePoints;
			uniform float lineWidth;
			uniform vec4 tint;
			
			in vec2 screenPos;
			in flat int lineIdx;
			out vec4 fragColor;
		
			float getSquaredDistanceToLine(vec2 a, vec2 b, vec2 pt)
			{
				vec2 lineDiff = b - a;
				float sqrdLineLength = dot(lineDiff, lineDiff);
				
				vec2 pointDiff = pt - a;
				if(sqrdLineLength < 0.0001f) // a == b
					return dot(pointDiff, pointDiff);
				else
				{
					float t = clamp(dot(pointDiff, lineDiff / sqrdLineLength), 0, 1);
					vec2 projPoint = a + lineDiff * t;
					vec2 toLineDiff = projPoint - pt;
					return dot(toLineDiff, toLineDiff);
				} 
			}

			void main()
			{
				// Get number of lines
				int numPoints = textureSize(linePoints);
				int numLines = numPoints - 1;
								
				// Find nearest line
				//// Distance to current line
				vec2 a = texelFetch(linePoints, lineIdx + 0).xy;				
				vec2 b = texelFetch(linePoints, lineIdx + 1).xy;
				
				float sqrdDist = getSquaredDistanceToLine(a, b, screenPos);

				//// Distance to previous line
				int prevLineIdx = max(0, lineIdx - 1);
				a = texelFetch(linePoints, prevLineIdx + 0).xy;				
				b = texelFetch(linePoints, prevLineIdx + 1).xy;
				
				sqrdDist = min(sqrdDist, getSquaredDistanceToLine(a, b, screenPos));
				
				//// Distance to next line
				int nextLineIdx = min(numLines - 1, lineIdx + 1);
				a = texelFetch(linePoints, nextLineIdx + 0).xy;				
				b = texelFetch(linePoints, nextLineIdx + 1).xy;
				
				sqrdDist = min(sqrdDist, getSquaredDistanceToLine(a, b, screenPos));
				
				float sqrdLineWidth = lineWidth * lineWidth;
				sqrdDist = max(sqrdDist - sqrdLineWidth, 0.0f);
				
				float featherWidth = 1.0f; // When changing this change the width of the line quads as well
				float filtered = sqrdDist / (featherWidth * featherWidth); // TODO - Use a different filter like Gaussian
				float weight = 1.0f - clamp(filtered, 0, 1);
				fragColor = vec4(tint.rgb, tint.a * weight);
			}
		};
	};
};