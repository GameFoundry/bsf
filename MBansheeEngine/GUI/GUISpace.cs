using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIFixedSpace : GUIElement
    {
        internal GUIFixedSpace(GUILayout parentLayout, int size)
        {
            Internal_CreateInstance(this, parentLayout, size);
        }

        internal override bool IsStatic()
        {
            return true;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFixedSpace instance, GUILayout parentLayout, int size);
    }

    public sealed class GUIFlexibleSpace : GUIElement
    {
        internal GUIFlexibleSpace(GUILayout parentLayout)
        {
            Internal_CreateInstance(this, parentLayout);
        }

        internal override bool IsStatic()
        {
            return true;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFlexibleSpace instance, GUILayout parentLayout);
    }
}
