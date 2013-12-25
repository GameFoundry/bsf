using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class GUILayoutX : GUILayout
    {
        internal GUILayoutX()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUILayoutX instance);
    }
}
