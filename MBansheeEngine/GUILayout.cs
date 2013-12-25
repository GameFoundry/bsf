using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class GUILayout : ScriptObject
    {
        internal GUILayout()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUILayout instance);
    }
}
