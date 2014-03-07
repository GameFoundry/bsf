using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUISkin : ScriptObject
    {
        public GUISkin()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISkin instance);
    }
}
