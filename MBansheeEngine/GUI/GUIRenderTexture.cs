using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIRenderTexture : GUIElement
    {
        private RenderTexture2D renderTexture;

        public RenderTexture2D RenderTexture
        {
            get
            {
                return renderTexture;
            }

            set
            {
                IntPtr texturePtr = IntPtr.Zero;
                if (value != null)
                    texturePtr = value.GetCachedPtr();

                renderTexture = value;
                Internal_SetTexture(mCachedPtr, texturePtr);
            }
        }

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIRenderTexture instance, IntPtr texture,
            string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, IntPtr texture);
    }
}
