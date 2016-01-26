using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public enum GUIImageScaleMode
	{
		StretchToFit,
		ScaleToFit,
		CropToFit,
		RepeatToFit
	};

    public sealed class GUITexture : GUIElement
    {
        public GUITexture(SpriteTexture texture, GUIImageScaleMode scale, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, scale, style, options);
        }

        public GUITexture(SpriteTexture texture, GUIImageScaleMode scale, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, scale, "", options);
        }

        public GUITexture(SpriteTexture texture, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, GUIImageScaleMode.StretchToFit, style, options);
        }

        public GUITexture(SpriteTexture texture, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, GUIImageScaleMode.StretchToFit, "", options);
        }

        public void SetTexture(SpriteTexture texture)
        {
            Internal_SetTexture(mCachedPtr, texture);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITexture instance, SpriteTexture texture,
            GUIImageScaleMode scale, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, SpriteTexture texture);
    }
}
