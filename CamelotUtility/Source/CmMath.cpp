/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmMath.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmRay.h"
#include "CmSphere.h"
#include "CmAABox.h"
#include "CmPlane.h"

namespace CamelotFramework
{
    const float Math::POS_INFINITY = std::numeric_limits<float>::infinity();
    const float Math::NEG_INFINITY = -std::numeric_limits<float>::infinity();
    const float Math::PI = (float)4.0f * std::atan(1.0f);
    const float Math::TWO_PI = (float)(2.0f * PI);
    const float Math::HALF_PI = (float)(0.5f * PI);
	const float Math::DEG2RAD = PI / 180.0f;
	const float Math::RAD2DEG = 180.0f / PI;
	const float Math::LOG2 = std::log(2.0f);

    Radian Math::acos(float val)
    {
        if (-1.0f < val)
        {
            if (val < 1.0f)
                return Radian(std::acos(val));
            else
                return Radian(0.0f);
        }
        else
        {
            return Radian(PI);
        }
    }

    Radian Math::asin(float val)
    {
        if (-1.0f < val)
        {
            if (val < 1.0f)
                return Radian(std::asin(val));
            else
                return Radian(HALF_PI);
        }
        else
        {
            return Radian(-HALF_PI);
        }
    }

    float Math::sign(float val)
    {
        if (val > 0.0f)
            return 1.0f;

        if (val < 0.0f)
            return -1.0f;

        return 0.0f;
    }

	float Math::invSqrt(float val)
	{
		return 1.0f/sqrt(val);
	}

    bool Math::approxEquals(float a, float b, float tolerance)
    {
        if (fabs(b-a) <= tolerance)
            return true;
        else
            return false;
    }

	Vector3 Math::calculateTriTangent(const Vector3& position1, const Vector3& position2, 
		const Vector3& position3, float u1, float v1, float u2, float v2, float u3, float v3)
	{
		Vector3 side0 = position1 - position2;
		Vector3 side1 = position3 - position1;

		// Calculate face normal
		Vector3 normal = side1.cross(side0);
		normal.normalize();

		// Now we use a formula to calculate the tangent. 
		float deltaV0 = v1 - v2;
		float deltaV1 = v3 - v1;
		Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
		tangent.normalize();

		// Calculate binormal
		float deltaU0 = u1 - u2;
		float deltaU1 = u3 - u1;
		Vector3 binormal = deltaU1 * side0 - deltaU0 * side1;
		binormal.normalize();

		// Now, we take the cross product of the tangents to get a vector which 
		// should point in the same direction as our normal calculated above. 
		// If it points in the opposite direction (the dot product between the normals is less than zero), 
		// then we need to reverse the s and t tangents. 
		// This is because the triangle has been mirrored when going from tangent space to object space.
		// reverse tangents if necessary.
		Vector3 tangentCross = tangent.cross(binormal);
		if (tangentCross.dot(normal) < 0.0f)
		{
			tangent = -tangent;
			binormal = -binormal;
		}

		return tangent;
	}
}
