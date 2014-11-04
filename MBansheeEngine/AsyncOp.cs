using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class AsyncOp : ScriptObject
    {
        internal AsyncOp()
        {
            Internal_CreateInstance(this);
        }

        public bool IsCompleted
        {
            get
            {
                bool value;
                Internal_IsComplete(mCachedPtr, out value);
                return value;
            }
        }

        public T GetReturnValue<T>()
        {
            return (T)Internal_GetReturnValue(mCachedPtr);
        }

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
