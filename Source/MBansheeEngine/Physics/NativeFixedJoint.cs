//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native FixedJoint class.
    /// <see cref="FixedJoint"/>
    /// </summary>
    internal class NativeFixedJoint : NativeJoint
    {
        public NativeFixedJoint(ScriptCommonJointData commonData)
        {
            Internal_CreateInstance(this, ref commonData);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeFixedJoint instance, ref ScriptCommonJointData commonData);
    }
}
