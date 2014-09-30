#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsCapsule.h"
#include "BsSphere.h"
#include "BsRect3.h"
#include "BsTorus.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleManager : public Module<HandleManager>
	{
	public:
		// TODO - Add a bunch of draw methods similar to GizmoManager
		//      - And a similar render method

		//void drawCube(const Vector3& position, const Vector3& extents);
		//void drawSphere(const Vector3& position, float radius);
		//void drawWireCube(const Vector3& position, const Vector3& extents);
		//void drawWireSphere(const Vector3& position, float radius);
		//void drawCone(const Vector3& base, const Vector3& normal, float height, float radius);
		//void drawLine(const Vector3& start, const Vector3& end);
		//void drawDisc(const Vector3& position, const Vector3& normal, float radius);
		//void drawWireDisc(const Vector3& position, const Vector3& normal, float radius);
		//void drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle);
		//void drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle);

		// TODO - Add update method that handles mouse input

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