using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    public class ScriptObject
    {
        protected IntPtr mCachedPtr;

        ~ScriptObject()
        {
            Internal_DestroyInstance(mCachedPtr);
        }
        
        [DllImport("__Internal")]
        private static extern void Internal_DestroyInstance(IntPtr nativeInstance);
    }
}
