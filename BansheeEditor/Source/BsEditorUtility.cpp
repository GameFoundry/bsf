#include "BsEditorUtility.h"
#include "BsSceneObject.h"
#include "BsCRenderable.h"

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
		bounds = AABox(Vector3::ZERO, Vector3::ZERO);
		if (object.isDestroyed())
			return false;

		bool foundOne = false;
		const Vector<HComponent>& components = object->getComponents();
		for (auto& component : components)
		{
			Bounds curBounds;
			if (component->calculateBounds(curBounds))
			{
				if (!foundOne)
				{
					bounds = curBounds.getBox();
					foundOne = true;
				}
				else
					bounds.merge(curBounds.getBox());
			}
			else
			{
				if (!foundOne)
					bounds = curBounds.getBox();
			}
		}

		return foundOne;
	}
}