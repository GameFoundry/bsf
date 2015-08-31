using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Object you may use to check on the results of an asynchronous operation. Contains uninitialized data until 
    /// <see cref="IsCompleted"/> returns true.
    /// </summary>
    public class AsyncOp : ScriptObject
    {
        /// <summary>
        /// Constructs a new async operation.
        /// </summary>
        internal AsyncOp() // Note: For internal runtime use only
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Checks has the asynchronous operation completed.
        /// </summary>
        public bool IsCompleted
        {
            get
            {
                bool value;
                Internal_IsComplete(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Retrieves the value returned by the async operation. Only valid if <see cref="IsCompleted"/> returns true.
        /// </summary>
        /// <typeparam name="T">Type of the return value. Caller must ensure to provide the valid type.</typeparam>
        /// <returns></returns>
        public T GetReturnValue<T>()
        {
            return (T)Internal_GetReturnValue(mCachedPtr);
        }

        /// <summary>
        /// Blocks the calling thread until asynchronous operation completes.
        /// </summary>
        public void BlockUntilComplete()
        {
            Internal_BlockUntilComplete(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_CreateInstance(AsyncOp managedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_IsComplete(IntPtr thisPtr, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern object Internal_GetReturnValue(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_BlockUntilComplete(IntPtr thisPtr);
    }
}
