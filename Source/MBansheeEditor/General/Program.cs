//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup BansheeEditor
     *  @{
     */

    /** @defgroup GUI-Editor GUI 
     *
     */

    /** @defgroup Input-Editor Input 
     *
     */

    /** @defgroup General General 
     *
     */

    /** @defgroup Inspectors Inspectors 
     *
     */

    /** @defgroup Script Script 
     *
     */

    /** @defgroup Tests Tests 
     *
     */

    /** @defgroup Utility-Editor Utility 
     *
     */

    /** @defgroup Window Window 
     *
     */

    /** @defgroup Windows Windows 
     *  @{
     */

    /** @defgroup Build Build 
     *
     */

    /** @defgroup Inspector Inspector 
     *
     */

    /** @defgroup Library Library 
     *
     */

    /** @defgroup Scene-Editor Scene 
     *  @{
     */

    /** @defgroup Gizmos Gizmos 
     *
     */

    /** @defgroup Handles Handles 
     *
     */

    /** @} */

    /** @defgroup Settings Settings 
     *
     */

    /** @} */

    /** @} */

    /** @cond Interop */

    /// <summary>
    /// Entry point to the editor.
    /// </summary>
    class Program
    {
        private static EditorApplication app;

        /// <summary>
        /// Called by the runtime whenever the editor assembly is loaded. This means initially when editor is started
        /// and every time assembly refresh occurs.
        /// </summary>
        static void OnInitialize()
        {
            app = new EditorApplication();
        }

        /// <summary>
        /// Called by the runtime when the editor is first started. Called after <see cref="OnInitialize"/>.
        /// </summary>
        static void OnEditorStartUp()
        {
            if (EditorSettings.AutoLoadLastProject)
            {
                string projectPath = EditorSettings.LastOpenProject;
                if (EditorApplication.IsValidProject(projectPath))
                    EditorApplication.LoadProject(projectPath);
                else
                    ProjectWindow.Open();
            }
            else
                ProjectWindow.Open();

            CodeEditorType activeCodeEditor = (CodeEditorType)EditorSettings.GetInt(SettingsWindow.ActiveCodeEditorKey, (int) CodeEditorType.None);
            CodeEditorType[] availableEditors = CodeEditor.AvailableEditors;
            if (Array.Exists(availableEditors, x => x == activeCodeEditor))
                CodeEditor.ActiveEditor = activeCodeEditor;
            else
            {
                if (availableEditors.Length > 0)
                    CodeEditor.ActiveEditor = availableEditors[0];
            }
        }

        /// <summary>
        /// Called 60 times per second by the runtime.
        /// </summary>
        static void OnEditorUpdate()
        {
            app.OnEditorUpdate();
        }

        /// <summary>
        /// Attempts to save the current scene, and keeps retrying if failed or until user cancels.
        /// </summary>
        static void TrySaveScene()
        {
            Action success = () =>
            {
                EditorApplication.SaveProject();
                EditorApplication.Quit();
            };

            EditorApplication.SaveScene(success, TrySaveScene);
        }

        /// <summary>
        /// Called when the user requests that the editor shuts down. You must manually close the editor from this
        /// method if you choose to accept the users request.
        /// </summary>
        static void OnEditorQuitRequested()
        {
            Action<DialogBox.ResultType> dialogCallback =
            (result) =>
            {
                if (result == DialogBox.ResultType.Yes)
                    TrySaveScene();
                else if (result == DialogBox.ResultType.No)
                {
                    EditorApplication.SaveProject();
                    EditorApplication.Quit();
                }
            };

            if (EditorApplication.IsSceneModified())
            {
                DialogBox.Open("Warning", "You current scene has modifications. Do you wish to save them first?",
                    DialogBox.Type.YesNoCancel, dialogCallback);
            }
            else
            {
                EditorApplication.SaveProject();
                EditorApplication.Quit();
            }
        }
    }

    /** @endcond */
}
