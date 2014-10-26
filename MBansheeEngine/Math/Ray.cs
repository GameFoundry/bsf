using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public class Ray
    {
		public Vector3 origin;
		public Vector3 direction;

        public Ray() {}

        public Ray(Vector3 origin, Vector3 direction)
        {
            this.origin = origin;
            this.direction = direction;
        }

		public static Vector3 operator*(Ray ray, float t) 
		{
            return ray.origin + ray.direction * t;
		}

        public void Transform(Matrix4 matrix)
        {
            Vector3 end = this * 1.0f;

		    origin = matrix.Multiply(origin);
		    end = matrix.Multiply(end);

		    direction = Vector3.Normalize(end - origin);
        }

        public void TransformAffine(Matrix4 matrix)
        {
            Vector3 end = this * 1.0f;

            origin = matrix.MultiplyAffine(origin);
            end = matrix.MultiplyAffine(end);

            direction = Vector3.Normalize(end - origin);
        }
    };
}
