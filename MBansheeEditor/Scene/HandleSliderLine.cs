using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class HandleSliderLine : HandleSlider
    {
        public HandleSliderLine(Handle parentHandle, Vector3 direction, float length,  bool fixedScale = true)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, direction, length, fixedScale);
        }

        public float Delta
        {
            get
            {
                float value;
                Internal_GetDelta(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderLine instance, Vector3 direction, float length, bool fixedScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out float value);
    }
}
