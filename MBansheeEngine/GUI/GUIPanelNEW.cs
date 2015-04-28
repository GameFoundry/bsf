using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIPanelNEW : GUILayout
    {
        public GUIPanelNEW(params GUIOption[] options)
        {
            Internal_CreateInstancePanel(this, options);
        }
    }
}
