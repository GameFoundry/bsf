using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays project and editor settings
    /// </summary>
    internal sealed class SettingsWindow : EditorWindow
    {
        private GUIFloatField defaultHandleSizeField;
        private GUIToggleField autoLoadLastProjectField;

        /// <summary>
        /// Opens the settings window if its not open already.
        /// </summary>
        [MenuItem("Tools/Settings", 9297, true)]
        private static void OpenSettingsWindow()
        {
            OpenWindow<SettingsWindow>();
        }

        /// <inheritdoc/>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Settings");
        }

        private void OnInitialize()
        {
            Width = 300;

            GUIToggle projectFoldout = new GUIToggle(new LocEdString("Project"), EditorStyles.Foldout);
            GUIToggle editorFoldout = new GUIToggle(new LocEdString("Editor"), EditorStyles.Foldout);

            defaultHandleSizeField = new GUIFloatField(new LocEdString("Handle size"), 200);
            defaultHandleSizeField.OnChanged += (x) => { EditorSettings.DefaultHandleSize = x; };

            autoLoadLastProjectField = new GUIToggleField(new LocEdString("Automatically load last project"), 200);
            autoLoadLastProjectField.OnChanged += (x) => { EditorSettings.AutoLoadLastProject = x; };

            GUILayout mainLayout = GUI.AddLayoutY();
            mainLayout.AddElement(projectFoldout);
            GUILayout projectLayoutOuterY = mainLayout.AddLayoutY();
            projectLayoutOuterY.AddSpace(5);
            GUILayout projectLayoutOuterX = projectLayoutOuterY.AddLayoutX();
            projectLayoutOuterX.AddSpace(5);
            GUILayout projectLayout = projectLayoutOuterX.AddLayoutY();
            projectLayoutOuterX.AddSpace(5);
            projectLayoutOuterY.AddSpace(5);

            mainLayout.AddElement(editorFoldout);
            GUILayout editorLayoutOuterY = mainLayout.AddLayoutY();
            editorLayoutOuterY.AddSpace(5);
            GUILayout editorLayoutOuterX = editorLayoutOuterY.AddLayoutX();
            editorLayoutOuterX.AddSpace(5);
            GUILayout editorLayout = editorLayoutOuterX.AddLayoutY();
            editorLayoutOuterX.AddSpace(5);
            editorLayoutOuterY.AddSpace(5);

            mainLayout.AddFlexibleSpace();

            editorLayout.AddElement(defaultHandleSizeField);
            editorLayout.AddElement(autoLoadLastProjectField);

            projectFoldout.Value = true;
            editorFoldout.Value = true;

            projectFoldout.OnToggled += (x) => projectLayout.Active = x;
            editorFoldout.OnToggled += (x) => editorLayout.Active = x;
        }

        private void OnEditorUpdate()
        {
            defaultHandleSizeField.Value = EditorSettings.DefaultHandleSize;
            autoLoadLastProjectField.Value = EditorSettings.AutoLoadLastProject;
        }
    }
}
