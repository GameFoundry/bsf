//********************************* bs::framework - Copyright 2019-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;

namespace bs
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Marks a <see cref="Color"/> field or property as high dynamic range.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class HDR : Attribute { }

    /** @} */
}
