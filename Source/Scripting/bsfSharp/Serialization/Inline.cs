//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;

namespace bs
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Displays class/struct members as if they were part of the parent class in the inspector.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class Inline : Attribute { }

    /** @} */
}
