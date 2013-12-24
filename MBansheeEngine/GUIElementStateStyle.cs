using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIElementStateStyle : ScriptObject
    {
        public GUIElementStateStyle()
        {
            Internal_CreateInstance(this);
        }

        public SpriteTexture texture
        {
            get { SpriteTexture value; Internal_GetTexture(mCachedPtr, out value); return value; }
            set { Internal_SetTexture(mCachedPtr, value); }
        }

        public Color textColor
        {
            get { Color value; Internal_GetTextColor(mCachedPtr, out value); return value; }
            set { Internal_SetTextColor(mCachedPtr, ref value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIElementStateStyle instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetTexture(IntPtr nativeInstance, out SpriteTexture value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, SpriteTexture value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetTextColor(IntPtr nativeInstance, out Color value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTextColor(IntPtr nativeInstance, ref Color value);
    };
}
