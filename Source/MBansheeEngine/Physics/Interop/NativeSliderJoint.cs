//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Interop
     *  @{
     */

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

        public NativeSliderJoint(ScriptCommonJointData commonData, ScriptSliderJointData data)
        {
            Internal_CreateInstance(this, ref commonData, ref data);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeSliderJoint instance, ref ScriptCommonJointData commonData,
            ref ScriptSliderJointData data);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPosition(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpeed(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLimit(IntPtr thisPtr, LimitLinearRange limit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableLimit(IntPtr thisPtr, bool enable);
    }

    /// <summary>
    /// Used for passing SliderJoint initialization data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    internal struct ScriptSliderJointData // Note: Must match C++ struct ScriptSliderJointData
    {
        public LimitLinearRange limit;
        public bool enableLimit;
    }

    /** @} */
}
