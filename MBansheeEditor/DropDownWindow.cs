using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class DropDownWindow : ScriptObject
    {
        private int width;
        private int height;

        public int Width
        {
            get { return width; }
            set { Internal_SetWidth(mCachedPtr, value); width = value; }
        }

        public int Height
        {
            get { return height; }
            set { Internal_SetHeight(mCachedPtr, value); height = value; }
        }

        protected GUIPanel GUI;

        public static T Open<T>(EditorWindow parent, Vector2I position) where T : DropDownWindow, new()
        {
            T window = new T();
            window.Initialize(parent, position);

            return window;
        }

        protected DropDownWindow(int width = 200, int height = 200)
        {
            this.width = width;
            this.height = height;
        }

        private void Initialize(EditorWindow parent, Vector2I position)
        {
            IntPtr parentPtr = IntPtr.Zero;
            if (parent != null)
                parentPtr = parent.GetCachedPtr();

            Internal_CreateInstance(this, parentPtr, position, width, height);
        }

        protected Vector2I ScreenToWindowPos(Vector2I screenPos)
        {
            Vector2I windowPos;
            Internal_ScreenToWindowPos(mCachedPtr, screenPos, out windowPos);
            return windowPos;
        }

        protected Vector2I WindowToScreenPos(Vector2I windowPos)
        {
            Vector2I screenPos;
            Internal_WindowToScreenPos(mCachedPtr, windowPos, out screenPos);
            return screenPos;
        }

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
