using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIRenderTexture : GUIElement
    {
        public GUIRenderTexture(RenderTexture2D texture, string style, params GUIOption[] options)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (texture != null)
                texturePtr = texture.GetCachedPtr();

            Internal_CreateInstance(this, texturePtr, style, options);
        }

        public GUIRenderTexture(RenderTexture2D texture, params GUIOption[] options)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (texture != null)
                texturePtr = texture.GetCachedPtr();

            Internal_CreateInstance(this, texturePtr, "", options);
        }

        public void SetTexture(RenderTexture2D texture)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (texture != null)
                texturePtr = texture.GetCachedPtr();

            Internal_SetTexture(mCachedPtr, texturePtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIRenderTexture instance, IntPtr texture,
            string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, IntPtr texture);
    }
}
