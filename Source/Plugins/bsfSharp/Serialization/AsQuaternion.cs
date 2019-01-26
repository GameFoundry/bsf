//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2010 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// When a quaternion is displayed in the inspector, by default it will be displayed as converted into euler angles.
    /// Use this attribute to force it to be displayed as a quaternion (4D value) with no conversion instead.
    /// </summary>
    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
    public sealed class AsQuaternion : Attribute { }

    /** @} */
}
