using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Quaternion
    {
        private struct EulerAngleOrderData
		{
            public EulerAngleOrderData(int a, int b, int c)
            {
                this.a = a;
                this.b = b;
                this.c = c;
            }

			public int a, b, c;
		};

        public static readonly Quaternion zero = new Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
        public static readonly Quaternion identity = new Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

        private static readonly float epsilon = 1e-03f;

        private static readonly EulerAngleOrderData[] EA_LOOKUP = new EulerAngleOrderData[6]
		    { new EulerAngleOrderData(0, 1, 2), new EulerAngleOrderData(0, 2, 1), new EulerAngleOrderData(1, 0, 2),
		      new EulerAngleOrderData(1, 2, 0), new EulerAngleOrderData(2, 0, 1), new EulerAngleOrderData(2, 1, 0) };

        public float x;
        public float y;
        public float z;
        public float w;

        public float this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0:
                        return x;
                    case 1:
                        return y;
                    case 2:
                        return z;
                    case 3:
                        return w;
                    default:
                        throw new IndexOutOfRangeException("Invalid Quaternion index.");
                }
            }
            set
            {
                switch (index)
                {
                    case 0:
                        x = value;
                        break;
                    case 1:
                        y = value;
                        break;
                    case 2:
                        z = value;
                        break;
                    case 3:
                        w = value;
                        break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Quaternion index.");
                }
            }
        }

        public Vector3 Right
        {
            get
            {
                float fTy = 2.0f*y;
                float fTz = 2.0f*z;
                float fTwy = fTy*w;
                float fTwz = fTz*w;
                float fTxy = fTy*x;
                float fTxz = fTz*x;
                float fTyy = fTy*y;
                float fTzz = fTz*z;

                return new Vector3(1.0f - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy);
            }
        }

        public Vector3 Up
        {
            get
            {
                float fTx = 2.0f * x;
                float fTy = 2.0f * y;
                float fTz = 2.0f * z;
                float fTwx = fTx * w;
                float fTwz = fTz * w;
                float fTxx = fTx * x;
                float fTxy = fTy * x;
                float fTyz = fTz * y;
                float fTzz = fTz * z;

                return new Vector3(fTxy - fTwz, 1.0f - (fTxx + fTzz), fTyz + fTwx);
            }
        }

        public Vector3 Forward
        {
            get
            {
                float fTx = 2.0f * x;
                float fTy = 2.0f * y;
                float fTz = 2.0f * z;
                float fTwx = fTx * w;
                float fTwy = fTy * w;
                float fTxx = fTx * x;
                float fTxz = fTz * x;
                float fTyy = fTy * y;
                float fTyz = fTz * y;

                return new Vector3(fTxz + fTwy, fTyz - fTwx, 1.0f - (fTxx + fTyy));
            }
        }

        public Quaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public static Quaternion operator* (Quaternion lhs, Quaternion rhs)
        {
            return new Quaternion((lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y), 
                (lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z), 
                (lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x), 
                (lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z));
        }

        public static Quaternion operator* (float lhs, Quaternion rhs)
        {
            return new Quaternion(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
        }

        public static Quaternion operator+ (Quaternion lhs, Quaternion rhs)
        {
            return new Quaternion(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
        }

        public static Quaternion operator- (Quaternion lhs, Quaternion rhs)
        {
            return new Quaternion(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
        }

        public static Quaternion operator- (Quaternion quat)
        {
            return new Quaternion(-quat.w, -quat.x, -quat.y, -quat.z);
        }

        public static bool operator== (Quaternion lhs, Quaternion rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
        }

        public static bool operator!= (Quaternion lhs, Quaternion rhs)
        {
            return !(lhs == rhs);
        }

        public static float Dot(Quaternion a, Quaternion b)
        {
            return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
        }

        public Vector3 Rotate(Vector3 point)
        {
            return ToRotationMatrix().Transform(point);
        }

        public void SetFromToRotation(Vector3 fromDirection, Vector3 toDirection)
        {
            SetFromToRotation(fromDirection, toDirection, Vector3.zero);
        }

        public void SetFromToRotation(Vector3 fromDirection, Vector3 toDirection, Vector3 fallbackAxis)
        {
		    fromDirection.Normalize();
		    toDirection.Normalize();

		    float d = Vector3.Dot(fromDirection, toDirection);

		    // If dot == 1, vectors are the same
            if (d >= 1.0f)
            {
                this = identity;
                return;
            }

            if (d < (1e-6f - 1.0f))
		    {
			    if (fallbackAxis != Vector3.zero)
			    {
				    // Rotate 180 degrees about the fallback axis
				    this = FromAxisAngle(fallbackAxis, MathEx.Pi * MathEx.Rad2Deg);
			    }
			    else
			    {
				    // Generate an axis
				    Vector3 axis = Vector3.Cross(Vector3.xAxis, fromDirection);
                    if (axis.sqrdMagnitude < ((1e-06f * 1e-06f))) // Pick another if collinear
					    axis = Vector3.Cross(Vector3.yAxis, fromDirection);
				    axis.Normalize();
                    this = FromAxisAngle(axis, MathEx.Pi * MathEx.Rad2Deg);
			    }
		    }
		    else
		    {
			    float s = MathEx.Sqrt((1+d)*2);
			    float invs = 1 / s;

			    Vector3 c = Vector3.Cross(fromDirection, toDirection);

			    x = c.x * invs;
			    y = c.y * invs;
			    z = c.z * invs;
			    w = s * 0.5f;
			    Normalize();
		    }
        }

        public float Normalize()
        {
            float len = w*w+x*x+y*y+z*z;
            float factor = 1.0f / (float)MathEx.Sqrt(len);

            x *= factor;
            y *= factor;
            z *= factor;
            w *= factor;
            return len;
        }

        public void Inverse()
        {
            float fNorm = w * w + x * x + y * y + z * z;
            if (fNorm > 0.0f)
            {
                float fInvNorm = 1.0f / fNorm;
                x *= -fInvNorm;
                y *= -fInvNorm;
                z *= -fInvNorm;
                w *= fInvNorm;
            }
            else
            {
                this = zero;
            }
        }

        public void SetLookRotation(Vector3 forward)
        {
            SetLookRotation(forward, Vector3.yAxis);
        }

        public void SetLookRotation(Vector3 forward, Vector3 up)
        {
            Quaternion forwardRot = FromToRotation(Vector3.zAxis, forward);
            Quaternion upRot = FromToRotation(Vector3.yAxis, up);

            this = forwardRot * upRot;
        }

        public static Quaternion Slerp(Quaternion from, Quaternion to, float t, bool shortestPath = false)
        {
            float cos = from.w*to.w + from.x*to.x + from.y*to.y + from.z*from.z;
            Quaternion quat;

            if (cos < 0.0f && shortestPath)
            {
                cos = -cos;
                quat = -to;
            }
            else
            {
                quat = to;
            }

            if (MathEx.Abs(cos) < (1 - epsilon))
            {
                // Standard case (slerp)
                float sin = MathEx.Sqrt(1 - (cos*cos));
                float angle = MathEx.Atan2(sin, cos);
                float invSin = 1.0f / sin;
                float coeff0 = MathEx.Sin((1.0f - t) * angle) * invSin;
                float coeff1 = MathEx.Sin(t * angle) * invSin;
                return coeff0 * from + coeff1 * quat;
            }
            else
            {
                // There are two situations:
                // 1. "p" and "q" are very close (fCos ~= +1), so we can do a linear
                //    interpolation safely.
                // 2. "p" and "q" are almost inverse of each other (fCos ~= -1), there
                //    are an infinite number of possibilities interpolation. but we haven't
                //    have method to fix this case, so just use linear interpolation here.
                Quaternion ret = (1.0f - t) * from + t * quat;

                // Taking the complement requires renormalization
                ret.Normalize();
                return ret;
            }
        }

        public static Quaternion RotateTowards(Quaternion from, Quaternion to, Degree maxDeg)
        {
            Degree num = Angle(from, to);
            if (num == 0.0f)
                return to;

            float t = MathEx.Min(1f, (float)(maxDeg / num));
            return Slerp(from, to, t);
        }

        public static Quaternion Inverse(Quaternion rotation)
        {
            Quaternion copy = rotation;
            copy.Inverse();

            return copy;
        }

        public static Degree Angle(Quaternion a, Quaternion b)
        {
            return (MathEx.Acos(MathEx.Min(MathEx.Abs(Dot(a, b)), 1.0f)) * 2.0f * MathEx.Rad2Deg);
        }

        public void ToAxisAngle(out Vector3 axis, out Degree angle)
        {
            float fSqrLength = x*x+y*y+z*z;
		    if (fSqrLength > 0.0f)
		    {
                angle = 2.0f * MathEx.Acos(w) * MathEx.Rad2Deg;
			    float fInvLength = MathEx.InvSqrt(fSqrLength);
			    axis.x = x*fInvLength;
			    axis.y = y*fInvLength;
			    axis.z = z*fInvLength;
		    }
		    else
		    {
			    // Angle is 0, so any axis will do
                angle = 0.0f;
			    axis.x = 1.0f;
			    axis.y = 0.0f;
			    axis.z = 0.0f;
		    }
        }

        // Returns angles in degrees
        public Vector3 ToEulerAngles(EulerAngleOrder order = EulerAngleOrder.XYZ)
        {
            Matrix3 matRot = ToRotationMatrix();
            return matRot.ToEulerAngles(order);
        }

        public Matrix3 ToRotationMatrix()
        {
            Matrix3 mat = new Matrix3();

            float tx = x + x;
            float ty = y + y;
            float fTz = z + z;
            float twx = tx * w;
            float twy = ty * w;
            float twz = fTz * w;
            float txx = tx * x;
            float txy = ty * x;
            float txz = fTz * x;
            float tyy = ty * y;
            float tyz = fTz * y;
            float tzz = fTz * z;

            mat[0, 0] = 1.0f - (tyy + tzz);
            mat[0, 1] = txy - twz;
            mat[0, 2] = txz + twy;
            mat[1, 0] = txy + twz;
            mat[1, 1] = 1.0f - (txx + tzz);
            mat[1, 2] = tyz - twx;
            mat[2, 0] = txz - twy;
            mat[2, 1] = tyz + twx;
            mat[2, 2] = 1.0f - (txx + tyy);

            return mat;
        }

        public static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection)
        {
            Quaternion q = new Quaternion();
            q.SetFromToRotation(fromDirection, toDirection);
            return q;
        }

        public static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection, Vector3 fallbackAxis)
        {
            Quaternion q = new Quaternion();
            q.SetFromToRotation(fromDirection, toDirection, fallbackAxis);
            return q;
        }

        public static Quaternion LookRotation(Vector3 forward)
        {
            Quaternion quat = new Quaternion();
            quat.SetLookRotation(forward);

            return quat;
        }

        public static Quaternion LookRotation(Vector3 forward, Vector3 up)
        {
            Quaternion quat = new Quaternion();
            quat.SetLookRotation(forward, up);

            return quat;
        }

        public static Vector3 ToEulerAngles(Quaternion rotation, EulerAngleOrder order = EulerAngleOrder.XYZ)
        {
            return rotation.ToEulerAngles(order);
        }

        public static void ToAxisAngle(Quaternion rotation, out Vector3 axis, out Degree angleDeg)
        {
            rotation.ToAxisAngle(out axis, out angleDeg);
        }

        public static Quaternion FromRotationMatrix(Matrix3 rotMatrix)
        {
            // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
            // article "Quaternion Calculus and Fast Animation".

            Quaternion quat = new Quaternion();
            float trace = rotMatrix.m00 + rotMatrix.m11 + rotMatrix.m22;
            float root;

            if (trace > 0.0f)
            {
                // |w| > 1/2, may as well choose w > 1/2
                root = MathEx.Sqrt(trace + 1.0f);  // 2w
                quat.w = 0.5f*root;
                root = 0.5f/root;  // 1/(4w)
                quat.x = (rotMatrix.m21 - rotMatrix.m12) * root;
                quat.y = (rotMatrix.m02 - rotMatrix.m20) * root;
                quat.z = (rotMatrix.m10 - rotMatrix.m01) * root;
            }
            else
            {
                // |w| <= 1/2
                int[] nextLookup = { 1, 2, 0 };
                int i = 0;

                if (rotMatrix.m11 > rotMatrix.m00)
                    i = 1;

                if (rotMatrix.m22 > rotMatrix[i, i])
                    i = 2;

                int j = nextLookup[i];
                int k = nextLookup[j];

                root = MathEx.Sqrt(rotMatrix[i,i] - rotMatrix[j, j] - rotMatrix[k, k] + 1.0f);

                quat[i] = 0.5f*root;
                root = 0.5f/root;

                quat.w = (rotMatrix[k, j] - rotMatrix[j, k]) * root;
                quat[j] = (rotMatrix[j, i] + rotMatrix[i, j]) * root;
                quat[k] = (rotMatrix[k, i] + rotMatrix[i, k]) * root;
            }

		    quat.Normalize();

            return quat;
        }

        public static Quaternion FromAxisAngle(Vector3 axis, Degree angleDeg)
        {
            Quaternion quat;

            float halfAngle = (float)(0.5f*angleDeg*MathEx.Deg2Rad);
            float sin = (float)MathEx.Sin(halfAngle);
            quat.w = (float)MathEx.Cos(halfAngle);
            quat.x = sin * axis.x;
            quat.y = sin * axis.y;
            quat.z = sin * axis.z;

            return quat;
        }

        public static Quaternion FromEuler(float xDeg, float yDeg, float zDeg, EulerAngleOrder order = EulerAngleOrder.XYZ)
        {
            EulerAngleOrderData l = EA_LOOKUP[(int)order];

            Quaternion[] quats = new Quaternion[3];
		    quats[0] = FromAxisAngle(Vector3.xAxis, xDeg);
		    quats[1] = FromAxisAngle(Vector3.yAxis, yDeg);
		    quats[2] = FromAxisAngle(Vector3.zAxis, zDeg);

            return quats[l.c]*(quats[l.a] * quats[l.b]);
        }

        /**
         * @note Angles in degrees.
         */
        public static Quaternion FromEuler(Vector3 euler, EulerAngleOrder order = EulerAngleOrder.XYZ)
        {
            return FromEuler(euler.x, euler.y, euler.z, order);
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2 ^ z.GetHashCode() >> 2 ^ w.GetHashCode() >> 1;
        }

        public override bool Equals(object other)
        {
            if (!(other is Quaternion))
                return false;

            Quaternion quat = (Quaternion)other;
            if (x.Equals(quat.x) && y.Equals(quat.y) && z.Equals(quat.z) && w.Equals(quat.w))
                return true;

            return false;
        }

        public override string ToString()
        {
            return String.Format("({0}, {1}, {2}, {3})", x, y, z, w);
        }
    }
}
