﻿//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;

namespace bs
{
    /** @addtogroup GUI_Engine
     *  @{
     */

    /// <summary>
    /// Various helper functionality that's useful when creating GUI elements.
    /// </summary>
    public class GUIUtility
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
        /// <param name="relativeTo">GUI layout the bounds will be relative to. If this is null or the provided layout is
        ///                          not a parent of the provided GUI element, the returned bounds will be relative to the
        ///                          first GUI panel parent instead.</param>
        /// <returns>Bounds of a GUI element relative to the provided GUI layout.</returns>
        public static Rect2I CalculateBounds(GUIElement element, GUILayout relativeTo = null)
        {
            IntPtr relativeToNative = IntPtr.Zero;
            if (relativeTo != null)
                relativeToNative = relativeTo.GetCachedPtr();

            Rect2I output;
            Internal_CalculateBounds(element.GetCachedPtr(), relativeToNative, out output);
            return output;
        }

        /// <summary>
        /// Calculates optimal content size for the provided text using the provided font and size. Size is calculated
        /// without word wrap.
        /// </summary>
        /// <param name="text">Text to calculate the bounds for.</param>
        /// <param name="font">Font that will be used for rendering the text.</param>
        /// <param name="fontSize">Size of individual characters in the font, in points.</param>
        /// <returns>Width/height required to display the text, in pixels.</returns>
        public static Vector2I CalculateTextBounds(string text, Font font, int fontSize)
        {
            Vector2I output;

            if (font != null && text != null)
            {
                IntPtr fontPtr = font.GetCachedPtr();
                Internal_CalculateTextBounds(text, fontPtr, fontSize, out output);
            }
            else
                output = new Vector2I();

            return output;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateOptimalSize(IntPtr element, out Vector2I output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateBounds(IntPtr element, IntPtr relativeTo, out Rect2I output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateTextBounds(string text, IntPtr fontPtr, int fontSize, out Vector2I output);
    }

    /** @} */
}
