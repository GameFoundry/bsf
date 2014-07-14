using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public class RectI
    {
        public RectI(int x, int y, int width, int height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }

		public static bool operator== (RectI lhs, RectI rhs)
		{
			return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height;
		}

		public static bool operator!= (RectI lhs, RectI rhs)
		{
			return !(lhs == rhs);
		}

        public override int GetHashCode()
        {
            int hash = 23;
            hash = hash * 31 + x.GetHashCode();
            hash = hash * 31 + y.GetHashCode();
            hash = hash * 31 + width.GetHashCode();
            hash = hash * 31 + height.GetHashCode();

            return hash;
        }

        public override bool Equals(object other)
        {
            if (!(other is RectI))
                return false;

            RectI rect = (RectI)other;
            if (x.Equals(rect.x) && y.Equals(rect.y) && width.Equals(rect.width) && height.Equals(rect.height))
                return true;

            return false;
        }

        int x, y, width, height;
    }
}
