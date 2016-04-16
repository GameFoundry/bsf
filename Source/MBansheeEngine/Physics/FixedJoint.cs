//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /** @addtogroup Physics
     *  @{
     */

    /// <summary>
    /// Physics joint that will maintain a fixed distance and orientation between its two attached bodies.
    /// </summary>
    public sealed class FixedJoint : Joint
    {
        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeFixedJoint joint = new NativeFixedJoint(commonData.@internal);

            return joint;
        }
    }

    /** @} */
}