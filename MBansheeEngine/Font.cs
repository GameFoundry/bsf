using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CharRange
    {
        public UInt32 start;
        public UInt32 end;
    }

    public sealed class Font : Resource // TODO - Dummy class
    {
        internal Font(bool constructNative)
        {
            if (constructNative)
                Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Font instance);
    }
}
