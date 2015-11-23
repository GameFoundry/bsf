using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for all editor windows. Editor window can be docked on the main editor window or float as a separate 
    /// window. User is allowed to reposition and resize the window as he wishes. Editor window GUI is fully customizable.
    /// </summary>
    public class EditorWindow : ScriptObject
    {
        /// <summary>
        /// Width of the window in pixels.
        /// </summary>
        public int Width
        {
            get { return Internal_GetWidth(mCachedPtr); }
        }

        /// <summary>
        /// Height of the window in pixels.
        /// </summary>
        public int Height
        {
            get { return Internal_GetHeight(mCachedPtr); }
        }

        /// <summary>
        /// Determines whether the editor window currently has keyboard focus (has been clicked on most recently).
        /// </summary>
        public bool HasFocus { get { return Internal_HasFocus(mCachedPtr); } }

        protected GUIPanel GUI;

        /// <summary>
        /// Opens an editor window. If window is already open it returns the existing instance.
        /// </summary>
        /// <typeparam name="T">Type of window to open.</typeparam>
        /// <returns>Instance of the open window.</returns>
        public static T OpenWindow<T>() where T : EditorWindow
        {
            return (T)Internal_CreateOrGetInstance(typeof(T).Namespace, typeof(T).Name);
        }

        /// <summary>
        /// Retrieves instance of an open window.
        /// </summary>
        /// <typeparam name="T">Type of window to retrieve the instance of.</typeparam>
        /// <returns>Instance of the winodow if it is open, null otherwise.</returns>
        public static T GetWindow<T>() where T : EditorWindow
        {
            return (T)Internal_GetInstance(typeof(T).Namespace, typeof(T).Name);
        }

        /// <summary>
        /// Converts coordinates in screen space to coordinates relative to the window.
        /// </summary>
        /// <param name="screenPos">Coordinates in screen space.</param>
        /// <returns>Coordinates relative to the window.</returns>
        public Vector2I ScreenToWindowPos(Vector2I screenPos)
        {
            Vector2I windowPos;
            Internal_ScreenToWindowPos(mCachedPtr, screenPos, out windowPos);
            return windowPos;
        }

        /// <summary>
        /// Converts coordinates relative to the window to screen space to coordinates.
        /// </summary>
        /// <param name="windowPos">Coordinates relative to the window.</param>
        /// <returns>Coordinates in screen space.</returns>
        public Vector2I WindowToScreenPos(Vector2I windowPos)
        {
            Vector2I screenPos;
            Internal_WindowToScreenPos(mCachedPtr, windowPos, out screenPos);
            return screenPos;
        }

        /// <summary>
        /// Triggered whenever the window size ranges.
        /// </summary>
        /// <param name="width">New width of the window in pixels.</param>
        /// <param name="height">New height of the window in pixels.</param>
        protected virtual void WindowResized(int width, int height)
        {

        }

        /// <summary>
        /// Triggered whenever the window gains or loses focus.
        /// </summary>
        /// <param name="inFocus">True if focus was gained, false otherwise.</param>
        protected virtual void FocusChanged(bool inFocus)
        {
            
        }

        /// <summary>
        /// Name of the window to display in the title bar.
        /// </summary>
        /// <returns>Name of the window to display in the title bar.</returns>
        protected virtual LocString GetDisplayName()
        {
            return GetType().Name;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern EditorWindow Internal_CreateOrGetInstance(string ns, string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern EditorWindow Internal_GetInstance(string ns, string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidth(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeight(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_HasFocus(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToWindowPos(IntPtr nativeInstance, Vector2I position, out Vector2I windowPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_WindowToScreenPos(IntPtr nativeInstance, Vector2I position, out Vector2I screenPos);
    }
}
