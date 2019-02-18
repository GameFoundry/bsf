//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;

namespace bs
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>Groups inspector field under a category foldout with the specified name.</summary>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class Category : Attribute
    {
        /// <summary>
        /// Creates a new category attribute.
        /// </summary>
        /// <param name="name">Name of the category.</param>
        public Category(string name)
        {
            this.name = name;
        }

#pragma warning disable 0414
        private string name;
#pragma warning restore 0414
    }

    /** @} */
}
