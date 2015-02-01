using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class HandleSliderDisc : HandleSlider
    {
        public HandleSliderDisc(Handle parentHandle, Vector3 normal, float radius, bool fixedScale = true)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, normal, radius, fixedScale);
        }

        public Degree Delta
        {
            get
            {
                float value;
                Internal_GetDelta(mCachedPtr, out value);
                return value;
            }
        }

        public Degree StartAngle
        {
            get
            {
                float value;
                Internal_GetStartAngle(mCachedPtr, out value);
                return value;
            }
        }

        public void SetCutoffPlane(Degree angle, bool enabled)
        {
            Internal_SetCutoffPlane(mCachedPtr, angle.Degrees, enabled);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderDisc instance, Vector3 normal, float radius, bool fixedScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetStartAngle(IntPtr nativeInstance, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCutoffPlane(IntPtr nativeInstance, float angle, bool enabled);
    }
}
