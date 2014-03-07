using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIFixedSpace : GUIElement
    {
        internal GUIFixedSpace(GUILayout parentLayout, int size)
            :base(parentLayout)
        {
            Internal_CreateInstance(this, parentLayout, size);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFixedSpace instance, GUILayout parentLayout, int size);
    }

    public sealed class GUIFlexibleSpace : GUIElement
    {
        internal GUIFlexibleSpace(GUILayout parentLayout)
            :base(parentLayout)
        {
            Internal_CreateInstance(this, parentLayout);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFlexibleSpace instance, GUILayout parentLayout);
    }
}
