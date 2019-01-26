//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup Utility
     *  @{
     */

    public partial class AsyncOp
    {
        /// <summary>
        /// Retrieves the value returned by the async operation. Only valid if <see cref="IsCompleted"/> returns true.
        /// </summary>
        /// <typeparam name="T">Type of the return value. Caller must ensure to provide the valid type.</typeparam>
        /// <returns>Return value of the async operation.</returns>
        public T GetReturnValue<T>()
        {
            return (T)Internal_getReturnValue(mCachedPtr);
        }
    }

    /** @} */
}
