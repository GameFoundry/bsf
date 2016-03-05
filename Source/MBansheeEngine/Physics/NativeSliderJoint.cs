//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native SliderJoint class.
    /// <see cref="SliderJoint"/>
    /// </summary>
    internal class NativeSliderJoint : NativeJoint
    {
        public float Position
        {
            get { return Internal_GetPosition(mCachedPtr); }
        }

        public float Speed
        {
            get { return Internal_GetSpeed(mCachedPtr); }
        }

        public LimitLinearRange Limit
        {
            set { Internal_SetLimit(mCachedPtr, value); }
        }

        public bool EnableLimit
        {
            set { Internal_SetEnableLimit(mCachedPtr, value); }
        }

        public NativeSliderJoint()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeSliderJoint instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPosition(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpeed(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLimit(IntPtr thisPtr, LimitLinearRange limit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableLimit(IntPtr thisPtr, bool enable);
    }
}
