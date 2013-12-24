using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIWidget : ScriptObject
    {
        public GUIWidget()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIWidget instance);
    }
}
