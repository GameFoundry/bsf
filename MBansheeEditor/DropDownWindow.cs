using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for all drop down window implementations. Drop down windows are temporary windows that open above all
    /// other GUI, and close as soon user clicks outside of them.
    /// </summary>
    public class DropDownWindow : ScriptObject
    {
        private int width;
        private int height;

        /// <summary>
        /// Width of the window in pixels.
        /// </summary>
        public int Width
        {
            get { return width; }
            set { Internal_SetWidth(mCachedPtr, value); width = value; }
        }

        /// <summary>
        /// Height of the window in pixels.
        /// </summary>
        public int Height
        {
            get { return height; }
            set { Internal_SetHeight(mCachedPtr, value); height = value; }
        }

        protected GUIPanel GUI;

        /// <summary>
        /// Opens a new drop down window at the specified location.
        /// </summary>
        /// <typeparam name="T">Type of the drop down window to open.</typeparam>
        /// <param name="parent">Parent editor window to open the drop down window in.</param>
        /// <param name="position">Position relative to the parent editor window at which to open the drop down window.
        ///                        </param>
        /// <returns>Instance of the opened drop down window.</returns>
        public static T Open<T>(EditorWindow parent, Vector2I position) where T : DropDownWindow, new()
        {
            T window = new T();
            window.Initialize(parent, position);

            return window;
        }

        /// <summary>
        /// Constructs a new drop down window.
        /// </summary>
        /// <param name="width">Width of the window in pixels.</param>
        /// <param name="height">Height of the window in pixels.</param>
        protected DropDownWindow(int width = 200, int height = 200)
        {
            this.width = width;
            this.height = height;
        }

        /// <summary>
        /// Initializes the drop down window after construction. Must be called before use.
        /// </summary>
        /// <param name="parent">Parent editor window to open the drop down window in.</param>
        /// <param name="position">Position relative to the parent editor window at which to open the drop down window.
        ///                        </param>
        private void Initialize(EditorWindow parent, Vector2I position)
        {
            IntPtr parentPtr = IntPtr.Zero;
            if (parent != null)
                parentPtr = parent.GetCachedPtr();

            Internal_CreateInstance(this, parentPtr, position, width, height);
        }

        /// <summary>
        /// Converts coordinates in screen space to coordinates relative to the drop down window.
        /// </summary>
        /// <param name="screenPos">Coordinates in screen space.</param>
        /// <returns>Coordinates relative to the drop down window.</returns>
        protected Vector2I ScreenToWindowPos(Vector2I screenPos)
        {
            Vector2I windowPos;
            Internal_ScreenToWindowPos(mCachedPtr, screenPos, out windowPos);
            return windowPos;
        }

        /// <summary>
        /// Converts coordinates relative to the drop down window to screen space to coordinates.
        /// </summary>
        /// <param name="windowPos">Coordinates relative to the drop down window.</param>
        /// <returns>Coordinates in screen space.</returns>
        protected Vector2I WindowToScreenPos(Vector2I windowPos)
        {
            Vector2I screenPos;
            Internal_WindowToScreenPos(mCachedPtr, windowPos, out screenPos);
            return screenPos;
        }

        /// <summary>
        /// Closes the drop down window.
        /// </summary>
        protected void Close()
        {
            Internal_Close(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(DropDownWindow instance, IntPtr parentWindow, Vector2I position, int width, int height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Close(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetWidth(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetHeight(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToWindowPos(IntPtr nativeInstance, Vector2I position, out Vector2I windowPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_WindowToScreenPos(IntPtr nativeInstance, Vector2I position, out Vector2I screenPos);
    }
}
