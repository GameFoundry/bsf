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
        public GUITexture(SpriteTexture texture, GUIImageScaleMode scale, bool transparent, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, scale, transparent, style, options);
        }

        public GUITexture(SpriteTexture texture, GUIImageScaleMode scale, bool transparent, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, scale, transparent, "", options);
        }

        public GUITexture(SpriteTexture texture, bool transparent, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, GUIImageScaleMode.StretchToFit, transparent, style, options);
        }

        public GUITexture(SpriteTexture texture, bool transparent, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, GUIImageScaleMode.StretchToFit, transparent, "", options);
        }

        public GUITexture(SpriteTexture texture, GUIImageScaleMode scale, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, scale, true, style, options);
        }

        public GUITexture(SpriteTexture texture, GUIImageScaleMode scale, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, scale, true, "", options);
        }

        public GUITexture(SpriteTexture texture, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, GUIImageScaleMode.StretchToFit, true, style, options);
        }

        public GUITexture(SpriteTexture texture, params GUIOption[] options)
        {
            Internal_CreateInstance(this, texture, GUIImageScaleMode.StretchToFit, true, "", options);
        }

        public void SetTexture(SpriteTexture texture)
        {
            Internal_SetTexture(mCachedPtr, texture);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITexture instance, SpriteTexture texture,
            GUIImageScaleMode scale, bool transparent, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, SpriteTexture texture);
    }
}
