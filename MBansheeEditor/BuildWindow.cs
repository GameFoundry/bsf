using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Provides options for customizing and activating the build process which will output an executable of the game for a 
    /// specific platform, as well as any required resources.
    /// </summary>
    [DefaultSize(300, 200)]
    internal sealed class BuildWindow : EditorWindow
    {
        /// <summary>
        /// Opens the build window if its not open already.
        /// </summary>
        [MenuItem("Tools/Build", 9296)]
        private static void OpenSettingsWindow()
        {
            OpenWindow<SettingsWindow>();
        }

        /// <inheritdoc/>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Build");
        }

        private void OnInitialize()
        {
            GUILayoutX splitLayout = GUI.AddLayoutX();
            GUILayoutY platformLayout = splitLayout.AddLayoutY();
            GUILayoutY optionsLayout = splitLayout.AddLayoutY();

            
            GUITextField titleField = new GUITextField(new LocEdString("Title"));
            GUITextureField iconField = new GUITextureField(new LocEdString("Icon"));
            GUIToggleField debugToggle = new GUIToggleField(new LocEdString("Debug"));
            GUIResourceField sceneField = new GUIResourceField(typeof(Prefab), new LocEdString("Startup scene"));
        }

        private void OnEditorUpdate()
        {

        }
    }
}
