#include "BsFBXImportData.h"

namespace BansheeEngine
{
	FBXImportNode::~FBXImportNode()
	{
		for (auto& child : children)
			bs_delete(child);
	}

	FBXImportScene::FBXImportScene()
		:rootNode(nullptr)
	{ }

	FBXImportScene::~FBXImportScene()
	{
		if (rootNode != nullptr)
			bs_delete(rootNode);

		for (auto& mesh : meshes)
			bs_delete(mesh);
	}

	float FBXAnimationCurve::evaluate(float time)
	{
		INT32 keyframeCount = (INT32)keyframes.size();
		if (keyframeCount == 0)
			return 0.0f;

		INT32 keyframeIdx = -1;
		for (INT32 i = 0; i < keyframeCount; i++)
		{
			if (time <= keyframes[i].time)
			{
				keyframeIdx = i;
				break;
			}
		}

		if (keyframeIdx == -1)
			keyframeIdx = keyframeCount - 1;

		if (keyframeIdx == 0)
			return keyframes[0].value;

		FBXKeyFrame& kfPrev = keyframes[keyframeIdx - 1];
		FBXKeyFrame& kfCur = keyframes[keyframeIdx];

		float delta = kfCur.time - kfPrev.time;
		float offset = time - kfPrev.time;
		float t = offset / delta;

		// Evaluate Cubic Hermite spline
		float t2 = t * t;
		float t3 = t2 * t;

		float x0 = (2 * t3 - 3 * t2 + 1) * kfPrev.value;
		float x1 = (t3 - 2 * t2 + t) * kfPrev.outTangent;
		float x2 = (-2 * t3 + 3 * t2) * kfCur.value;
		float x3 = (t3 - t2) * kfCur.inTangent;

		return x0 + x1 + x2 + x3;
	}
}