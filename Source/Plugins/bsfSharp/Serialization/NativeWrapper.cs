//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Attribute that signifies that property is a wrapper to some native value. It's getter/setter calls are native
    /// interop calls which then modify the internal value.
    /// </summary>
    [AttributeUsage(AttributeTargets.Property)]
    public sealed class NativeWrapper : Attribute { }

    /** @} */
}
