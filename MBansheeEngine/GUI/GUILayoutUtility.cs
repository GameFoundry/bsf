using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class GUILayoutUtility
    {
        public static Vector2 CalculateOptimalSize(GUIElement element)
        {
            return Internal_CalculateOptimalSize(element);
        }

        public static RectI CalculateBounds(GUIElement element)
        {
            return Internal_CalculateBounds(element);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2 Internal_CalculateOptimalSize(GUIElement element);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern RectI Internal_CalculateBounds(GUIElement element);
    }
}
