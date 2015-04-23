using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct GUIOption
    {
        internal enum Type
        {
            FixedWidth,
            FlexibleWidth,
            FixedHeight,
            FlexibleHeight
        }

        internal int min, max;
        internal Type type;

        public static GUIOption FixedWidth(int width)
        {
            GUIOption option = new GUIOption();
            option.min = option.max = width;
            option.type = Type.FixedWidth;

            return option;
        }

        public static GUIOption FixedHeight(int height)
        {
            GUIOption option = new GUIOption();
            option.min = option.max = height;
            option.type = Type.FixedHeight;

            return option;
        }

        public static GUIOption FlexibleWidth(int minWidth = 0, int maxWidth = 0)
        {
            GUIOption option = new GUIOption();
            option.min = minWidth;
            option.max = maxWidth;
            option.type = Type.FlexibleWidth;

            return option;
        }

        public static GUIOption FlexibleHeight(int minHeight = 0, int maxHeight = 0)
        {
            GUIOption option = new GUIOption();
            option.min = minHeight;
            option.max = maxHeight;
            option.type = Type.FlexibleHeight;

            return option;
        }
    }
}
