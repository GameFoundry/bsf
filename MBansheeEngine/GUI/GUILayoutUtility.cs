using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class GUILayoutUtility
    {
        public static Vector2I CalculateOptimalSize(GUIElement element)
        {
            Vector2I output;
            Internal_CalculateOptimalSize(element.GetCachedPtr(), out output);
            return output;
        }

        public static Rect2I CalculateBounds(GUIElement element, GUIPanel relativeTo = null)
        {
            IntPtr relativeToNative = IntPtr.Zero;
            if (relativeTo != null)
                relativeToNative = relativeTo.GetCachedPtr();

            Rect2I output;
            Internal_CalculateBounds(element.GetCachedPtr(), relativeToNative, out output);
            return output;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateOptimalSize(IntPtr element, out Vector2I output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateBounds(IntPtr element, IntPtr relativeTo, out Rect2I output);
    }
}
