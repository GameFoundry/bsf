using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public class Rect2
    {
        public Rect2(float x, float y, float width, float height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }

        public static bool operator ==(Rect2 lhs, Rect2 rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height;
        }

        public static bool operator !=(Rect2 lhs, Rect2 rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object other)
        {
            if (!(other is Rect2))
                return false;

            Rect2 rect = (Rect2)other;
            if (x.Equals(rect.x) && y.Equals(rect.y) && width.Equals(rect.width) && height.Equals(rect.height))
                return true;

            return false;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public float x, y, width, height;
    }
}
