using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUISliderH : GUIElement
    {
        public delegate void OnChangedDelegate(float percent);

        public event OnChangedDelegate OnChanged;

        public float Percent
        {
            get { return Internal_GetPercent(mCachedPtr); }
            set { Internal_SetPercent(mCachedPtr, value); }
        }

        public GUISliderH(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        public GUISliderH(string style)
        {
            Internal_CreateInstance(this, style, new GUIOption[0]);
        }

        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
        }

        private void DoOnChanged(float percent)
        {
            if (OnChanged != null)
                OnChanged(percent);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISliderH instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPercent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPercent(IntPtr nativeInstance, float percent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }

    public sealed class GUISliderV : GUIElement
    {
        public delegate void OnChangedDelegate(float percent);

        public event OnChangedDelegate OnChanged;

        public float Percent
        {
            get { return Internal_GetPercent(mCachedPtr); }
            set { Internal_SetPercent(mCachedPtr, value); }
        }

        public GUISliderV(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        public GUISliderV(string style)
        {
            Internal_CreateInstance(this, style, new GUIOption[0]);
        }

        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
        }

        private void DoOnChanged(float percent)
        {
            if (OnChanged != null)
                OnChanged(percent);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISliderV instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPercent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPercent(IntPtr nativeInstance, float percent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
