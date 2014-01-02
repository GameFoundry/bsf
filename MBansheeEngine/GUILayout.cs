using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : ScriptObject
    {
        public GUILabel AddLabel(GUIContent content, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUILabel(this, content, style, options);
        }

        public GUILabel AddLabel(GUIContent content, GUIElementStyle style)
        {
            return new GUILabel(this, content, style, new GUIOption[0]);
        }

        public GUILabel AddLabel(GUIContent content, params GUIOption[] options)
        {
            return new GUILabel(this, content, null, options);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromArea(GUILayout instance, GUIArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromLayout(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayout(GUILayout instance, GUILayout parentLayout);
    }
}
