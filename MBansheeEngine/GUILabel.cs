using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILabel : ScriptObject
    {
        internal GUILabel(GUILayout parentLayout, string name, GUIElementStyle style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, parentLayout, name, style, options);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUILabel instance, GUILayout layout, string name, GUIElementStyle style, GUIOption[] options);
    }
}
