#pragma once

#include "BsEditorPrerequisites.h"
#include "BsCapsule.h"
#include "BsSphere.h"
#include "BsRect3.h"
#include "BsTorus.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderManager
	{
	public:
		HandleSliderManager();
		~HandleSliderManager();

		void update(const Vector2I& inputPos, const Ray& inputRay, const Matrix4& viewMatrix, bool pressed);
		bool isSliderActive() const;

		void _registerCapsuleCollider(const Capsule& collider, HandleSlider* slider);
		void _registerSphereCollider(const Sphere& collider, HandleSlider* slider);
		void _registerRectCollider(const Rect3& collider, HandleSlider* slider);
		void _registerTorusCollider(const Torus& collider, HandleSlider* slider);
		void _unregisterSlider(HandleSlider* slider);

	private:
		UnorderedSet<HandleSlider*> mSliders;

		UnorderedMap<HandleSlider*, Capsule> mCapsuleColliders;
		UnorderedMap<HandleSlider*, Sphere> mSphereColliders;
		UnorderedMap<HandleSlider*, Rect3> mRectColliders;
		UnorderedMap<HandleSlider*, Torus> mTorusColliders;
	};
}