using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUISkin : Resource
    {
        // Constructor for runtime use only (dummy parameter to differentiate from the normal constructor)
        private GUISkin(bool dummy)
        {  }

        public GUISkin()
        {
            Internal_CreateInstance(this);
        }

        public string[] StyleNames
        {
            get { return Internal_GetStyleNames(mCachedPtr); }
        }

        public GUIElementStyle GetStyle(string name)
        {
            return Internal_GetStyle(mCachedPtr, name);
        }

        public void SetStyle(string name, GUIElementStyle style)
        {
            IntPtr stylePtr = IntPtr.Zero;
            if (style != null)
                stylePtr = style.GetCachedPtr();

            Internal_SetStyle(mCachedPtr, name, stylePtr);
        }

        public void RemoveStyle(string name)
        {
            Internal_RemoveStyle(mCachedPtr, name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISkin instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GUIElementStyle Internal_GetStyle(IntPtr thisPtr, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetStyle(IntPtr thisPtr, string name, IntPtr style);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RemoveStyle(IntPtr thisPtr, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetStyleNames(IntPtr thisPtr);
    }
}
