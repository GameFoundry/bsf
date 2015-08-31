using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Helper class that performs various operations related to
    /// GUILayout and GUI element sizing/placement.
    /// </summary>
    public class GUILayoutUtility
    {
        /// <summary>
        /// Calculates optimal size of a GUI element.
        /// </summary>
        /// <param name="element">GUI element to calculate the optimal size for.</param>
        /// <returns>Size that allows the GUI element to properly display all of its content.</returns>
        public static Vector2I CalculateOptimalSize(GUIElement element)
        {
            Vector2I output;
            Internal_CalculateOptimalSize(element.GetCachedPtr(), out output);
            return output;
        }

        /// <summary>
        /// Calculates the bounds of a GUI element. This is similar to <see cref="GUIElement.Bounds"/> but allows you to
        /// returns bounds relative to a specific parent GUI panel.
        /// </summary>
        /// <param name="element">Elements to calculate the bounds for.</param>
        /// <param name="relativeTo">GUI panel the bounds will be relative to. If this is null or the provided panel is not
        ///                          a parent of the provided GUI element, the returned bounds will be relative to the 
        ///                          first GUI panel parent instead.</param>
        /// <returns>Bounds of a GUI element relative to the provided GUI panel.</returns>
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
