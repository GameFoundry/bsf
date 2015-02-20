using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIProgressBar : GUIElement
    {
        public float Percent
        {
            get { return Internal_GetPercent(mCachedPtr); }
            set { Internal_SetPercent(mCachedPtr, value); }
        }

        public GUIProgressBar(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        public GUIProgressBar(string style = "")
        {
            Internal_CreateInstance(this, style, new GUIOption[0]);
        }

        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIProgressBar instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPercent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPercent(IntPtr nativeInstance, float percent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
