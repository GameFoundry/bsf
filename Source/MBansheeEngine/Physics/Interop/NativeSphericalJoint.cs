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
    /// Wrapper around the native SphericalJoint class.
    /// <see cref="SphericalJoint"/>
    /// </summary>
    internal class NativeSphericalJoint : NativeJoint
    {
        public LimitConeRange Limit
        {
            set { Internal_SetLimit(mCachedPtr, value); }
        }

        public bool EnableLimit
        {
            set { Internal_SetEnableLimit(mCachedPtr, value); }
        }

        public NativeSphericalJoint(ScriptCommonJointData commonData, ScriptSphericalJointData data)
        {
            Internal_CreateInstance(this, ref commonData, ref data);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeSphericalJoint instance, 
            ref ScriptCommonJointData commonData, ref ScriptSphericalJointData data);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLimit(IntPtr thisPtr, LimitConeRange limit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableLimit(IntPtr thisPtr, bool enable);
    }

    /// <summary>
    /// Used for passing SphericalJoint initialization data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptSphericalJointData // Note: Must match C++ struct ScriptSphericalJointData
    {
        public LimitConeRange limit;
        public bool enableLimit;
    }

    /** @} */
}
