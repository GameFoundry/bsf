using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUI : GUIBase
    {
        internal GUI()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUI instance);
    }
}
