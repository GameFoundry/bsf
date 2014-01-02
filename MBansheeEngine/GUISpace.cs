using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIFixedSpace : ScriptObject
    {
        internal GUIFixedSpace(GUILayout parentLayout, int size)
        {
            Internal_CreateInstance(this, parentLayout, size);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFixedSpace instance, GUILayout parentLayout, int size);
    }

    public sealed class GUIFlexibleSpace : ScriptObject
    {
        internal GUIFlexibleSpace(GUILayout parentLayout)
        {
            Internal_CreateInstance(this, parentLayout);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFlexibleSpace instance, GUILayout parentLayout);
    }
}
