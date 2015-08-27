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

        }

        private void OnInitialize()
        {
            Title = "Project Manager";

            Width = 500;
            Height = 250;

            GUILayout vertLayout = GUI.AddLayoutY();
            
            vertLayout.AddSpace(5);
            GUILayout firstRow = vertLayout.AddLayoutX();
            vertLayout.AddFlexibleSpace();
            GUILayout secondRow = vertLayout.AddLayoutX();
            vertLayout.AddSpace(15);
            GUILayout thirdRow = vertLayout.AddLayoutX();
            vertLayout.AddFlexibleSpace();
            GUILayout fourthRow = vertLayout.AddLayoutX();
            vertLayout.AddSpace(5);

            projectInputBox = new GUITextField(new LocEdString("Project path"), 70, false, "", GUIOption.FixedWidth(425));
            projectInputBox.Value = EditorSettings.LastOpenProject;

            GUIButton openBtn = new GUIButton(new LocEdString("Open"), GUIOption.FixedWidth(50));
            openBtn.OnClick += OpenProject;

            firstRow.AddSpace(5);
            firstRow.AddElement(projectInputBox);
            firstRow.AddSpace(15);
            firstRow.AddElement(openBtn);
            firstRow.AddSpace(5);

            GUILabel recentProjectsLabel = new GUILabel(new LocEdString("Recent projects:"));

            secondRow.AddSpace(5);
            secondRow.AddElement(recentProjectsLabel);
            secondRow.AddFlexibleSpace();

            recentProjectsArea = new GUIScrollArea(GUIOption.FixedWidth(405), GUIOption.FixedHeight(140));
            thirdRow.AddSpace(5 + 10);
            thirdRow.AddElement(recentProjectsArea);
            thirdRow.AddSpace(15 + 10);

            GUILayout browseBtnLayout = thirdRow.AddLayoutY();
            GUIButton browseBtn = new GUIButton(new LocEdString("Browse"), GUIOption.FixedWidth(50));
            browseBtn.OnClick += BrowseClicked;
            browseBtnLayout.AddElement(browseBtn);
            browseBtnLayout.AddFlexibleSpace();
            thirdRow.AddSpace(5);

            autoLoadToggle = new GUIToggle("");
            autoLoadToggle.Value = EditorSettings.AutoLoadLastProject;

            GUILabel autoLoadLabel = new GUILabel(new LocEdString("Automatically load last open project"));

            GUIButton createBtn = new GUIButton(new LocEdString("Create"), GUIOption.FixedWidth(50));
            createBtn.OnClick += CreateClicked;

            fourthRow.AddSpace(5);
            fourthRow.AddElement(autoLoadToggle);
            fourthRow.AddElement(autoLoadLabel);
            fourthRow.AddFlexibleSpace();
            fourthRow.AddElement(createBtn);
            fourthRow.AddSpace(5);

            RefreshRecentProjects();

            // Add scroll area background
            GUIPanel scrollAreaBgPanel = GUI.AddPanel(1);

            GUITexture scrollAreaBgTex = new GUITexture(null, true, EditorStyles.ScrollAreaBg);
            scrollAreaBgPanel.AddElement(scrollAreaBgTex);

            Rect2I bounds = vertLayout.Bounds;
            Rect2I scrollAreaBounds = recentProjectsArea.Bounds;

            scrollAreaBounds.x -= 10;
            scrollAreaBounds.y += bounds.y - 10;
            scrollAreaBounds.height += 20;
            scrollAreaBounds.width += 20;

            scrollAreaBgTex.Bounds = scrollAreaBounds;
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
                LocString message = new LocEdString("Provided project path \"{0}\" doesn't contain a valid project.");
                message.setParameter(0, projectPath);

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
