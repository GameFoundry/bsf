#include "BsEditorUtility.h"
#include "BsSceneObject.h"
#include "BsRenderable.h"

namespace BansheeEngine
{
	AABox EditorUtility::calculateBounds(const HSceneObject& object)
	{
		Vector<HSceneObject> objects = { object };

		return calculateBounds(objects);
	}

	AABox EditorUtility::calculateBounds(const Vector<HSceneObject>& objects)
	{
		if (objects.size() == 0)
			return AABox(Vector3::ZERO, Vector3::ZERO);

		AABox bounds = AABox(Vector3::INF, -Vector3::INF);
		bool gotOneMesh = false;

		for (auto& object : objects)
		{
			AABox meshBounds;
			if (calculateMeshBounds(object, meshBounds))
			{
				bounds.merge(meshBounds);

				gotOneMesh = true;
			}
		}

		if (!gotOneMesh)
		{
			for (auto& object : objects)
				bounds.merge(object->getWorldPosition());
		}

		return bounds;
	}

	bool EditorUtility::calculateMeshBounds(const HSceneObject& object, AABox& bounds)
	{
		if (object->hasComponent<Renderable>())
		{
			HRenderable renderable = object->getComponent<Renderable>();
			bounds = renderable->getBounds().getBox();

			return true;
		}

		return false;
	}
}