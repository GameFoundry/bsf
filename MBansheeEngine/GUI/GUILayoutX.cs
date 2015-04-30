using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUILayoutX : GUILayout
    {
        public GUILayoutX(params GUIOption[] options)
        {
            Internal_CreateInstanceX(this, options);
        }
    }
}
