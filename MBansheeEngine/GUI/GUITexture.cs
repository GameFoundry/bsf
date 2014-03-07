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
        internal GUITexture(GUILayout parentLayout, SpriteTexture texture, GUIImageScaleMode scale, GUIElementStyle style, params GUIOption[] options)
            :base(parentLayout)
        {
            Internal_CreateInstance(this, parentLayout, texture, scale, style, options);
        }

        public void SetTexture(SpriteTexture texture)
        {
            Internal_SetTexture(mCachedPtr, texture);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITexture instance, GUILayout layout, SpriteTexture texture, 
            GUIImageScaleMode scale, GUIElementStyle style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture(IntPtr nativeInstance, SpriteTexture texture);
    }
}
