using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    class Program
    {
        private static EditorApplication app;

        static void OnInitialize()
        {
            app = new EditorApplication();
        }

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

        static void OnEditorUpdate()
        {
            app.OnEditorUpdate();
        }
    }
}
