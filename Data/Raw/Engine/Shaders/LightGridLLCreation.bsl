#include "$ENGINE$\GBuffer.bslinc"
#include "$ENGINE$\PerCameraData.bslinc"
#include "$ENGINE$\LightingCommon.bslinc"

Blocks =
{
	Block PerCamera : auto("PerCamera");
};

Technique
 : inherits("GBuffer")
 : inherits("PerCameraData")
 : inherits("LightingCommon") =
{
	Language = "HLSL11";
	
	Pass =
	{
		Compute = 
		{
			cbuffer Params : register(b0)
			{
				// Offsets at which specific light types begin in gLights buffer
				// Assumed directional lights start at 0
				// x - offset to point lights, y - offset to spot lights, z - total number of lights
				uint3 gLightOffsets;			
				uint gNumCells;
				uint3 gGridSize;
				uint gMaxNumLightsPerCell;
			}
			
			StructuredBuffer<LightData> gLights : register(t0);
			RWBuffer<uint> gLinkedListCounter : register(u0);
			RWBuffer<uint> gLinkedListHeads : register(u1);
			RWBuffer<uint2> gLinkedList : register(u2);
			
			float convertToDeviceZ(float viewZ)
			{
				return (gDeviceZToWorldZ.x - viewZ * gDeviceZToWorldZ.y) / viewZ;
			}
			
			float calcViewZFromCellZ(uint cellZ)
			{
				// TODO - Need better Z distribution. Currently I uniformly distribute in view space, but this
				// results in very elongated cells along Z
				float viewZ = gNearFar.x + (gNearFar.y - gNearFar.x) * cellZ / (float)gGridSize.z;
				
				return viewZ;
			}
		
			void calcCellAABB(uint3 cellIdx, out float3 center, out float3 extent)
			{
				// Convert grid XY coordinates to clip coordinates
				float2 a = 2.0f / gGridSize.xy;
			
				float3 ndcMin;
				float3 ndcMax;
			
				ndcMin.xy = cellIdx.xy * a - float2(1.0f, 1.0f);
				ndcMax.xy = (cellIdx.xy + 1) * a - float2(1.0f, 1.0f);
			
				// Flip Y depending on render API, depending if Y in NDC is facing up or down
				float flipY = sign(gMatProj[1][1]);
				ndcMin.y *= flipY;
				ndcMax.y *= flipY;
				
				// Because we're viewing along negative Z, farther end is the minimum
				float viewZMin = calcViewZFromCellZ(cellIdx.z + 1);
				float viewZMax = calcViewZFromCellZ(cellIdx.z);
			
				ndcMin.z = convertToDeviceZ(viewZMin);
				ndcMax.z = convertToDeviceZ(viewZMax);
			
				float4 clipMin = mul(gMatInvProj, float4(ndcMin, 1.0f));
				float4 clipMax = mul(gMatInvProj, float4(ndcMax, 1.0f));
				
				float3 viewMin = float3(clipMin.xy / clipMin.w, viewZMin);
				float3 viewMax = float3(clipMax.xy / clipMax.w, viewZMax);				
				
				extent = (viewMax - viewMin) * 0.5f;
				center = viewMin + extent;
			}
		
			[numthreads(THREADGROUP_SIZE, THREADGROUP_SIZE, THREADGROUP_SIZE)]
			void main(
				uint3 groupId : SV_GroupID,
				uint3 groupThreadId : SV_GroupThreadID,
				uint3 dispatchThreadId : SV_DispatchThreadID)
			{
				uint2 viewportMax = gViewportRectangle.xy + gViewportRectangle.zw;

				// Ignore pixels out of valid range
				if (all(dispatchThreadId.xy >= viewportMax))
					return;
					
				uint maxNumLinks = gNumCells * gMaxNumLightsPerCell;	
				uint cellIdx = (dispatchThreadId.z * gGridSize.y + dispatchThreadId.y) * gGridSize.x + dispatchThreadId.x;
				
				float3 cellCenter;
				float3 cellExtent;
				calcCellAABB(dispatchThreadId, cellCenter, cellExtent);
				
				for(uint type = 1; type < 3; ++type)
				{
					uint lightOffset = gLightOffsets[type - 1];
					uint lightEnd = gLightOffsets[type];
					for(uint i = lightOffset; i < lightEnd; ++i)
					{
						float4 lightPosition = mul(gMatView, float4(gLights[i].position, 1.0f));
						float lightRadius = gLights[i].radius;
						
						// Calculate distance from box to light
						float3 distances = max(abs(lightPosition - cellCenter) - cellExtent, 0);
						float distSqrd = dot(distances, distances);
						
						if(distSqrd <= (lightRadius * lightRadius))
						{
							uint nextLink;
							InterlockedAdd(gLinkedListCounter[0], 1U, nextLink);
							
							if(nextLink < maxNumLinks)
							{
								uint prevLink;
								InterlockedExchange(gLinkedListHeads[cellIdx], nextLink, prevLink);
								
								gLinkedList[nextLink] = uint2(i, prevLink);
							}
						}
					}
				}
			}
		};
	};
};

Technique
 : inherits("GBuffer")
 : inherits("PerCameraData")
 : inherits("LightingCommon") =
{
	Language = "GLSL";
	
	Pass =
	{
		Compute = 
		{
			layout (local_size_x = THREADGROUP_SIZE, local_size_y = THREADGROUP_SIZE, local_size_z = THREADGROUP_SIZE) in;
		
			layout(binding = 0, std140) uniform Params
			{
				// Offsets at which specific light types begin in gLights buffer
				// Assumed directional lights start at 0
				// x - offset to point lights, y - offset to spot lights, z - total number of lights
				uvec3 gLightOffsets;			
				uint gNumCells;
				uvec3 gGridSize;
				uint gMaxNumLightsPerCell;
			};
			
			layout(std430, binding = 1) buffer gLights
			{
				LightData[] gLightsData;
			};
			
			layout(binding = 2, r32ui) uniform uimageBuffer gLinkedListCounter;
			layout(binding = 3, r32ui) uniform uimageBuffer gLinkedListHeads;
			layout(binding = 4, rg32ui) uniform uimageBuffer gLinkedList;
			
			float convertToDeviceZ(float viewZ)
			{
				return (gDeviceZToWorldZ.x - viewZ * gDeviceZToWorldZ.y) / viewZ;
			}
			
			float calcViewZFromCellZ(uint cellZ)
			{
				// TODO - Need better Z distribution. Currently I uniformly distribute in view space, but this
				// results in very elongated cells along Z
				float viewZ = gNearFar.x + (gNearFar.y - gNearFar.x) * cellZ / float(gGridSize.z);
				
				return viewZ;
			}
		
			void calcCellAABB(uvec3 cellIdx, out vec3 center, out vec3 extent)
			{
				// Convert grid XY coordinates to clip coordinates
				vec2 a = 2.0f / gGridSize.xy;
			
				vec3 ndcMin;
				vec3 ndcMax;
			
				ndcMin.xy = cellIdx.xy * a - vec2(1.0f, 1.0f);
				ndcMax.xy = (cellIdx.xy + 1) * a - vec2(1.0f, 1.0f);
			
				// Flip Y depending on render API, depending if Y in NDC is facing up or down
				float flipY = sign(gMatProj[1][1]);
				ndcMin.y *= flipY;
				ndcMax.y *= flipY;
				
				// Because we're viewing along negative Z, farther end is the minimum
				float viewZMin = calcViewZFromCellZ(cellIdx.z + 1);
				float viewZMax = calcViewZFromCellZ(cellIdx.z);
			
				ndcMin.z = convertToDeviceZ(viewZMin);
				ndcMax.z = convertToDeviceZ(viewZMax);
			
				vec4 clipMin = gMatInvProj * vec4(ndcMin, 1.0f);
				vec4 clipMax = gMatInvProj * vec4(ndcMax, 1.0f);
				
				vec3 viewMin = vec3(clipMin.xy / clipMin.w, viewZMin);
				vec3 viewMax = vec3(clipMax.xy / clipMax.w, viewZMax);				
				
				extent = (viewMax - viewMin) * 0.5f;
				center = viewMin + extent;
			}

			void main()
			{
				uvec2 viewportMax = gViewportRectangle.xy + gViewportRectangle.zw;

				// Ignore pixels out of valid range
				if (all(greaterThanEqual(gl_GlobalInvocationID.xy, viewportMax)))
					return;
					
				uint maxNumLinks = gNumCells * gMaxNumLightsPerCell;	
				uint cellIdx = (gl_GlobalInvocationID.z * gGridSize.y + gl_GlobalInvocationID.y) * gGridSize.x + gl_GlobalInvocationID.x;
				
				vec3 cellCenter;
				vec3 cellExtent;
				calcCellAABB(gl_GlobalInvocationID, cellCenter, cellExtent);
				
				for(uint type = 1; type < 3; ++type)
				{
					uint lightOffset = gLightOffsets[type - 1];
					uint lightEnd = gLightOffsets[type];
					for(uint i = lightOffset; i < lightEnd; ++i)
					{
						vec4 lightPosition = gMatView * vec4(gLightsData[i].position, 1.0f);
						float lightRadius = gLightsData[i].radius;
						
						// Calculate distance from box to light
						vec3 distances = max(abs(lightPosition.xyz - cellCenter) - cellExtent, 0);
						float distSqrd = dot(distances, distances);
						
						if(distSqrd <= (lightRadius * lightRadius))
						{
							uint nextLink = imageAtomicAdd(gLinkedListCounter, 0, 1U);
							if(nextLink < maxNumLinks)
							{
								uint prevLink = imageAtomicExchange(gLinkedListHeads, int(cellIdx), nextLink);
								
								imageStore(gLinkedList, int(nextLink), uvec4(i, prevLink, 0, 0));
							}
						}
					}
				}
			}
		};
	};
};