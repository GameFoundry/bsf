using System;
using System.Runtime.InteropServices;

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

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(GUIElementStateStyle instance);

        [DllImport("__Internal")]
        private static extern void Internal_GetTexture(IntPtr nativeInstance, out SpriteTexture value);
        [DllImport("__Internal")]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, SpriteTexture value);

        [DllImport("__Internal")]
        private static extern void Internal_GetTextColor(IntPtr nativeInstance, out Color value);
        [DllImport("__Internal")]
        private static extern void Internal_SetTextColor(IntPtr nativeInstance, ref Color value);
    };
}
