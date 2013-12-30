using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILabel : ScriptObject
    {
        internal GUILabel(GUILayout parentLayout, HString value, GUIElementStyle style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, parentLayout, value, style, options);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUILabel instance, GUILayout layout, HString value, GUIElementStyle style, GUIOption[] options);
    }
}
