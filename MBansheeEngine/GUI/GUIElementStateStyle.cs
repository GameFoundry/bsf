using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Container for texture and text color used in a particular GUI element style.
    /// </summary>
    public sealed class GUIElementStateStyle : ScriptObject
    {
        /// <summary>
        /// Constructs a new GUI element state style with default values.
        /// </summary>
        public GUIElementStateStyle()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Texture used by the GUI element style state. Can be null.
        /// </summary>
        public SpriteTexture Texture
        {
            get { SpriteTexture value; Internal_GetTexture(mCachedPtr, out value); return value; }
            set { Internal_SetTexture(mCachedPtr, value); }
        }

        /// <summary>
        /// Color applied to the text when GUI element style state is active.
        /// </summary>
        public Color TextColor
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
