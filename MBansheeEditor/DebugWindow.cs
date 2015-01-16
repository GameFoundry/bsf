using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal class DebugWindow : EditorWindow
    {
        private void OnInitialize()
        {
            GUIButton refreshAssembly = new GUIButton("Refresh assembly");
            refreshAssembly.OnClick += RefreshAssembly_OnClick;

            GUI.layout.AddElement(refreshAssembly);
        }

        void RefreshAssembly_OnClick()
        {
            Internal_RefreshAssembly();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_RefreshAssembly();
    }
}
