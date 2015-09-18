using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
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
        static void OnEditorLoad()
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
        }

        /// <summary>
        /// Called 60 times per second by the runtime.
        /// </summary>
        static void OnEditorUpdate()
        {
            app.OnEditorUpdate();
        }
    }
}
