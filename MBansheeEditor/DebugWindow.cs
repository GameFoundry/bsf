using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
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

            GUIButton compileGame = new GUIButton("Compile game assembly");
            compileGame.OnClick += CompileGame_OnClick;

            GUI.layout.AddElement(refreshAssembly);
            GUI.layout.AddElement(compileGame);

            GUIButton testExplicitBtn = new GUIButton("TESTING EXPLICIT");
            testExplicitBtn.Bounds = compileGame.Bounds;
            GUIArea overlay = GUI.AddArea(0, 0, Width, Height, -1, GUILayoutType.Explicit);
            overlay.layout.AddElement(testExplicitBtn);
        }

        [MenuItem("Window/Test", ButtonModifier.ShiftAlt, ButtonCode.A)]
        private static void TestMenuItem()
        {
            ColorPicker.Show();
        }

        void RefreshAssembly_OnClick()
        {
            Internal_RefreshAssembly();
        }

        void CompileGame_OnClick()
        {
            CompilerInstance ci = ScriptCompiler.CompileAsync(ScriptAssemblyType.Game, PlatformType.Windows, true, "D:\\AssemblyOutput");

            while (!ci.IsDone)
            {
                Debug.Log("Compiling...");
                Thread.Sleep(50);
            }

            Debug.Log("COMPILATION DONE!");

            for(int i = 0; i < ci.ErrorMessages.Length; i++)
                Debug.Log("ERROR: " + ci.ErrorMessages[i].file + ": " + ci.ErrorMessages[i].line + " - " + ci.ErrorMessages[i].message);

            for (int i = 0; i < ci.WarningMessages.Length; i++)
                Debug.Log("WARNING: " + ci.WarningMessages[i].file + ": " + ci.WarningMessages[i].line + " - " + ci.WarningMessages[i].message);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_RefreshAssembly();
    }
}
