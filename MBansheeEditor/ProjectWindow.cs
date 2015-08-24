using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class ProjectWindow : ModalWindow
    {
        private GUITextField projectInputBox;
        private GUIScrollArea recentProjectsArea;
        private GUIToggle autoLoadToggle;

        public static ProjectWindow Open()
        {
            return new ProjectWindow();
        }

        protected ProjectWindow()
            : base(false)
        {
            Title = "Project Manager";

            Width = 500;
            Height = 350;
        }

        private void OnInitialize()
        {
            GUILayout vertLayout = GUI.AddLayoutY();
            GUILayout firstRow = vertLayout.AddLayoutX();
            GUILayout secondRow = vertLayout.AddLayoutX();
            GUILayout thirdRow = vertLayout.AddLayoutX();

            projectInputBox = new GUITextField(new LocEdString("Project path"), 70);
            projectInputBox.Value = EditorSettings.LastOpenProject;

            GUIButton openBtn = new GUIButton(new LocEdString("Open"));
            openBtn.OnClick += OpenProject;

            firstRow.AddElement(projectInputBox);
            firstRow.AddElement(openBtn);

            recentProjectsArea = new GUIScrollArea();
            secondRow.AddElement(recentProjectsArea);

            GUILayout browseBtnLayout = secondRow.AddLayoutY();
            GUIButton browseBtn = new GUIButton(new LocEdString("Browse"));
            browseBtn.OnClick += BrowseClicked;
            browseBtnLayout.AddElement(browseBtn);
            browseBtnLayout.AddFlexibleSpace();

            autoLoadToggle = new GUIToggle("");
            autoLoadToggle.Value = EditorSettings.AutoLoadLastProject;

            GUILabel autoLoadLabel = new GUILabel(new LocEdString("Automatically load last open project"));

            GUIButton createBtn = new GUIButton(new LocEdString("Create"));
            createBtn.OnClick += CreateClicked;

            thirdRow.AddElement(autoLoadToggle);
            thirdRow.AddElement(autoLoadLabel);
            thirdRow.AddFlexibleSpace();
            thirdRow.AddElement(createBtn);

            RefreshRecentProjects();
        }

        void OpenProject()
        {
            string projectPath = projectInputBox.Value;

            if (EditorApplication.IsValidProject(projectPath))
            {
                EditorSettings.AutoLoadLastProject = autoLoadToggle.Value;

                Close();
                EditorApplication.LoadProject(projectPath);
            }
            else
            {
                // Remove invalid project from recent projects list
                RecentProject[] recentProjects = EditorSettings.RecentProjects;
                for (int i = 0; i < recentProjects.Length; i++)
                {
                    if (PathEx.Compare(recentProjects[i].path, projectPath))
                    {
                        RecentProject[] newRecentProjects = new RecentProject[recentProjects.Length - 1];
                        int idx = 0;
                        for (int j = 0; j < recentProjects.Length; j++)
                        {
                            if (i == j)
                                continue;

                            newRecentProjects[idx] = recentProjects[j];
                            idx++;
                        }

                        EditorSettings.RecentProjects = newRecentProjects;
                        EditorSettings.Save();

                        break;
                    }
                }

                // Warn user
                LocString message = new LocEdString("Provided project path \"") + projectPath +
                                    new LocEdString("\" doesn't contain a valid project.");

                DialogBox.Open(new LocEdString("Error"), message, DialogBox.Type.OK);
            }
        }

        void BrowseClicked()
        {
            string projectPath = projectInputBox.Value;
            if (!Directory.Exists(projectPath))
                projectPath = Directory.GetCurrentDirectory();

            string selectedPath;
            if (BrowseDialog.OpenFolder(projectPath, "", out selectedPath))
                projectInputBox.Value = selectedPath;
        }

        void CreateClicked()
        {
            string projectPath = projectInputBox.Value;
            if (!Directory.Exists(projectPath))
                projectPath = Directory.GetCurrentDirectory();

            string selectedPath;
            if (BrowseDialog.OpenFolder(projectPath, "", out selectedPath))
            {
                projectInputBox.Value = selectedPath;

                EditorApplication.CreateProject(selectedPath);
                OpenProject();
            }
        }

        private void RefreshRecentProjects()
        {
            GUILayout scrollLayout = recentProjectsArea.Layout;
            while(scrollLayout.GetNumChildren() > 0)
                scrollLayout.GetChild(0).Destroy();

            RecentProject[] recentProjects = EditorSettings.RecentProjects;
            Array.Sort(recentProjects, (a, b) => a.accessTimestamp.CompareTo(b.accessTimestamp));

            GUIToggleGroup grp = new GUIToggleGroup();
            for (int i = 0; i < recentProjects.Length; i++)
            {
                string projectPath = recentProjects[i].path;

                GUIToggle entryBtn = new GUIToggle(projectPath, grp, EditorStyles.SelectableLabel);
                entryBtn.OnClick += () => OnEntryClicked(projectPath);
                entryBtn.OnDoubleClick += () => OnEntryDoubleClicked(projectPath);

                scrollLayout.AddElement(entryBtn);
            }
        }

        void OnEntryClicked(string path)
        {
            projectInputBox.Value = path;
        }

        void OnEntryDoubleClicked(string path)
        {
            projectInputBox.Value = path;
            OpenProject();
        }
    }
}
