#include "BsTorus.h"
#include "BsRay.h"
#include "BsMath.h"

namespace BansheeEngine
{
	Torus::Torus()
		:outerRadius(0.0f), innerRadius(0.0f)
	{ }

	Torus::Torus(float outerRadius, float innerRadius)
		:outerRadius(outerRadius), innerRadius(innerRadius)
	{ }

	std::pair<bool, float> Torus::intersects(const Ray& ray) const
	{
		const Vector3& org = ray.getOrigin();
		const Vector3& dir = ray.getDirection();

		float a = org.dot(dir);
		float b = org.dot(org);

		float outerSqrd = outerRadius*outerRadius;
		float innerSqrd = innerRadius*innerRadius;
		float K = a - innerSqrd - outerSqrd;

		float E = 1.0f;
		float D = 4 * b;
		float C = 2 * (2 * b*b + K + 2 * outerSqrd*dir.z*dir.z);
		float B = 4 * (K*b + 2 * outerSqrd*org.z*dir.z);
		float A = K*K + 4 * outerSqrd*(org.z*org.z - innerSqrd);

		float roots[4];
		UINT32 numRoots = Math::solveQuartic(A, B, C, D, E, roots);

		if (numRoots > 0)
		{
			float nearestT = std::numeric_limits<float>::max();

			for (UINT32 i = 0; i < numRoots; i++)
			{
				float t = roots[i];
				float x = org.x + t*dir.x;
				float y = org.x + t*dir.y;
				float l = outerRadius*(Math::PI / 2 - Math::atan2(y, x).valueRadians());

				if (l >= 0 && t < nearestT)
					nearestT = t;
			}

			return std::make_pair(true, nearestT);
		}

		return std::make_pair(false, 0.0f);
	}
}