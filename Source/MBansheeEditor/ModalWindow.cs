//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base implementation of a window that when open doesn't allow you to interact with other windows. Modal windows 
    /// are similar to editor windows but cannot be docked, and are meant to be used for temporary operations like dialog 
    /// boxes and progress bars.
    /// </summary>
    public class ModalWindow : ScriptObject
    {
        /// <summary>
        /// Width of the window in pixels.
        /// </summary>
        public int Width
        {
            get { return Internal_GetWidth(mCachedPtr); }
            set { Internal_SetWidth(mCachedPtr, value); }
        }

        /// <summary>
        /// Height of the window in pixels.
        /// </summary>
        public int Height
        {
            get { return Internal_GetHeight(mCachedPtr); }
            set { Internal_SetHeight(mCachedPtr, value); }
        }

        protected GUIPanel GUI;

        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        protected ModalWindow()
        {
            Internal_CreateInstance(this, false);
        }

        /// <summary>
        /// Creates a new modal window.
        /// </summary>
        /// <param name="allowCloseButton">Should the close button be displayed.</param>
        protected ModalWindow(bool allowCloseButton)
        {
            Internal_CreateInstance(this, allowCloseButton);
        }

        /// <summary>
        /// Converts coordinates in screen space to coordinates relative to the window.
        /// </summary>
        /// <param name="screenPos">Coordinates in screen space.</param>
        /// <returns>Coordinates relative to the window.</returns>
        protected Vector2I ScreenToWindowPos(Vector2I screenPos)
        {
            Vector2I windowPos;
            Internal_ScreenToWindowPos(mCachedPtr, ref screenPos, out windowPos);
            return windowPos;
        }

        /// <summary>
        /// Converts coordinates relative to the window to screen space to coordinates.
        /// </summary>
        /// <param name="windowPos">Coordinates relative to the window.</param>
        /// <returns>Coordinates in screen space.</returns>
        protected Vector2I WindowToScreenPos(Vector2I windowPos)
        {
            Vector2I screenPos;
            Internal_WindowToScreenPos(mCachedPtr, ref windowPos, out screenPos);
            return screenPos;
        }

        /// <summary>
        /// Triggered whenever the window size ranges.
        /// </summary>
        /// <param name="width">New width of the window in pixels.</param>
        /// <param name="height">New height of the window in pixels.</param>
        protected virtual void OnWindowResized(int width, int height)
        {

        }

        /// <summary>
        /// Closes the modal window.
        /// </summary>
        protected void Close()
        {
            Internal_Close(mCachedPtr);
        }

        /// <summary>
        /// Allows you to set the mame of the window to display in the title bar.
        /// </summary>
        public LocString Title
        {
            set { Internal_SetTitle(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ModalWindow instance, bool allowCloseButton);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Close(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTitle(IntPtr nativeInstance, LocString title);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidth(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_SetWidth(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeight(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_SetHeight(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToWindowPos(IntPtr nativeInstance, ref Vector2I position, out Vector2I windowPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_WindowToScreenPos(IntPtr nativeInstance, ref Vector2I position, out Vector2I screenPos);
    }
}
