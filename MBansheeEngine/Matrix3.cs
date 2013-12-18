using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public class Matrix3
    {
        public float[,] m = new float[3,3];

        public float this[int row, int column]
        {
            get
            {
                return m[row, column];
            }
            set
            {
                m[row, column] = value;
            }
        }

        public float this[int index]
        {
            get
            {
                return (float)m.GetValue(index);
            }
            set
            {
                m.SetValue(value, index);
            }
        }

        public Vector3 Transform(Vector3 vec)
        {
            // TODO
            return Vector3.zero;
        }

        public Vector3 ToEulerAngles(EulerAngleOrder order)
        {
            // TODO - Angles returned must be in degrees
            return Vector3.zero;
        }
    }
}
