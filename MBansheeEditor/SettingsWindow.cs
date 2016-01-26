//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays project and editor settings.
    /// </summary>
    [DefaultSize(300, 200)]
    internal sealed class SettingsWindow : EditorWindow
    {
        internal const string ActiveCodeEditorKey = "__ActiveCodeEditor";

        private GUIFloatField defaultHandleSizeField;
        private GUIToggleField autoLoadLastProjectField;
        private GUIListBoxField codeEditorField;
        private GUIIntField fpsLimitField;
        private GUISliderField mouseSensitivityField;

        /// <summary>
        /// Opens the settings window if its not open already.
        /// </summary>
        [MenuItem("Tools/Settings", 9250, true)]
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
            GUIToggle projectFoldout = new GUIToggle(new LocEdString("Project"), EditorStyles.Foldout);
            GUIToggle editorFoldout = new GUIToggle(new LocEdString("Editor"), EditorStyles.Foldout);

            defaultHandleSizeField = new GUIFloatField(new LocEdString("Handle size"), 200);
            defaultHandleSizeField.OnChanged += (x) => { EditorSettings.DefaultHandleSize = x; };

            autoLoadLastProjectField = new GUIToggleField(new LocEdString("Automatically load last project"), 200);
            autoLoadLastProjectField.OnChanged += (x) => { EditorSettings.AutoLoadLastProject = x; };

            CodeEditorType[] availableEditors = CodeEditor.AvailableEditors;
            Array.Resize(ref availableEditors, availableEditors.Length + 1);
            availableEditors[availableEditors.Length - 1] = CodeEditorType.None;

            string[] availableEditorNames = new string[availableEditors.Length];
            for (int i = 0; i < availableEditors.Length; i++)
                availableEditorNames[i] = Enum.GetName(typeof (CodeEditorType), availableEditors[i]);

            codeEditorField = new GUIListBoxField(availableEditorNames, new LocEdString("Code editor"), 200);
            codeEditorField.OnSelectionChanged += x =>
            {
                EditorSettings.SetInt(ActiveCodeEditorKey, (int)availableEditors[x]);
                CodeEditor.ActiveEditor = availableEditors[x];
            };

            fpsLimitField = new GUIIntField(new LocEdString("FPS limit"), 200);
            fpsLimitField.OnConfirmed += () => EditorSettings.FPSLimit = fpsLimitField.Value;
            fpsLimitField.OnFocusLost += () => EditorSettings.FPSLimit = fpsLimitField.Value;

            mouseSensitivityField = new GUISliderField(0.2f, 2.0f, new LocEdString("Mouse sensitivity"));
            mouseSensitivityField.OnChanged += (x) => EditorSettings.MouseSensitivity = x;

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
            editorLayout.AddElement(codeEditorField);
            editorLayout.AddElement(fpsLimitField);
            editorLayout.AddElement(mouseSensitivityField);

            projectFoldout.Value = true;
            editorFoldout.Value = true;

            projectFoldout.OnToggled += (x) => projectLayout.Active = x;
            editorFoldout.OnToggled += (x) => editorLayout.Active = x;
        }

        private void OnEditorUpdate()
        {
            defaultHandleSizeField.Value = EditorSettings.DefaultHandleSize;
            autoLoadLastProjectField.Value = EditorSettings.AutoLoadLastProject;
            fpsLimitField.Value = EditorSettings.FPSLimit;
            mouseSensitivityField.Value = EditorSettings.MouseSensitivity;

            CodeEditorType[] availableEditors = CodeEditor.AvailableEditors;
            int idx = Array.IndexOf(availableEditors, CodeEditor.ActiveEditor);
            if (idx != -1)
                codeEditorField.Index = idx;
        }
    }
}
