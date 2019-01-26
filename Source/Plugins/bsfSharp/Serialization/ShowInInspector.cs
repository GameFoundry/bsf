//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Attribute that forces a field or a property to be visible in the inspector window in editor. Normally public fields
    /// are show in the inspector by default, and you can use this attribute to show private/protected/internal fields, or
    /// to show properties (never shown by default) as well. Type of the field/property must be a serializable type or
    /// a type marked with this attribute itself.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Class | AttributeTargets.Struct)]
    public sealed class ShowInInspector : Attribute
    {
    }

    /** @} */
}