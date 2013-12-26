using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : ScriptObject
    {
        public GUILabel AddLabel(string name, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUILabel(this, name, style, options);
        }

        public GUILabel AddLabel(string name, GUIElementStyle style)
        {
            return new GUILabel(this, name, style, new GUIOption[0]);
        }

        public GUILabel AddLabel(string name, params GUIOption[] options)
        {
            return new GUILabel(this, name, null, options);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromArea(GUILayout instance, GUIArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromLayout(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayout(GUILayout instance, GUILayout parentLayout);
    }
}
