using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class HandleSliderDisc : HandleSlider
    {
        public HandleSliderDisc(Handle parentHandle, Vector3 normal, float radius, bool fixedScale = true, float snapValue = 0.0f)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, normal, radius, fixedScale);
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
        public Quaternion NewRotation
        {
            get
            {
                Quaternion value;
                Internal_GetNewRotation(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 DeltaDirection
        {
            get
            {
                Vector3 value;
                Internal_GetDeltaDirection(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderDisc instance, Vector3 normal, float radius, bool fixedScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDeltaDirection(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetNewRotation(IntPtr nativeInstance, out Quaternion value);
    }
}
