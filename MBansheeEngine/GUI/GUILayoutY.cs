using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutY : GUILayout
    {
        internal GUILayoutY(GUIScrollArea parentArea)
        {
            Internal_CreateInstanceYFromScrollArea(this, parentArea);
        }

        public GUILayoutY(params GUIOption[] options)
        {
            Internal_CreateInstanceY(this, options);
        }
    }
}
