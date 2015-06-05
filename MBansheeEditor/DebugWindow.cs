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
        bool debugSlowDown = false;

        private void OnInitialize()
        {
            GUIButton refreshAssembly = new GUIButton("Refresh assembly");
            refreshAssembly.OnClick += RefreshAssembly_OnClick;

            GUIButton compileGame = new GUIButton("Compile game assembly");
            compileGame.OnClick += CompileGame_OnClick;

            GUIButton openColorPicker = new GUIButton("Color picker");
            openColorPicker.OnClick += OpenColorPicker;

            GUILayout horzLayout = GUI.AddLayoutX();

            horzLayout.AddElement(refreshAssembly);
            horzLayout.AddElement(compileGame);
            horzLayout.AddElement(openColorPicker);

            GUIButton testExplicitBtn = new GUIButton("TESTING EXPLICIT");
            testExplicitBtn.Bounds = compileGame.Bounds;
            GUIPanel overlay = GUI.AddPanel(-1);
            overlay.SetWidth(Width);
            overlay.SetHeight(Height);
            overlay.AddElement(testExplicitBtn);
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

        void OpenColorPicker()
        {
            ColorPicker.Show();
            debugSlowDown = true;
        }

        void OnEditorUpdate()
        {
            //if (debugSlowDown)
            //    Thread.Sleep(5000);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_RefreshAssembly();
    }
}
