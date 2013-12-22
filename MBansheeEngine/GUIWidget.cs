using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public sealed class GUIWidget : ScriptObject
    {
        public GUIWidget()
        {
            Internal_CreateInstance(this);
        }

        ~GUIWidget()
        {
            Internal_DestroyInstance(mCachedPtr);
        }

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(GUIWidget instance);

        [DllImport("__Internal")]
        private static extern void Internal_DestroyInstance(IntPtr nativeInstance);
    }
}
