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
        private PlatformType selectedPlatform;
        private GUIScrollArea optionsScrollArea;
        private bool buildScheduled;

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

            GUIToggleGroup platformToggleGroup = new GUIToggleGroup();

            PlatformType[] availablePlatforms = BuildManager.AvailablePlatforms;
            for (int i = 0; i < availablePlatforms.Length; i++)
            {
                PlatformType currentPlatform = availablePlatforms[i];
                string platformName = Enum.GetName(typeof(PlatformType), currentPlatform);

                GUIToggle platformToggle = new GUIToggle(new LocEdString(platformName), platformToggleGroup, EditorStyles.Button);
                platformToggle.OnToggled += x => OnSelectedPlatformChanged(currentPlatform, x);
                platformLayout.AddElement(platformToggle);

                if (currentPlatform == BuildManager.ActivePlatform)
                {
                    platformToggle.Value = true;
                    selectedPlatform = currentPlatform;
                }
            }

            platformLayout.AddFlexibleSpace();

            GUIButton changePlatformBtn = new GUIButton(new LocEdString("Set active"));
            platformLayout.AddElement(changePlatformBtn);
            changePlatformBtn.OnClick += ChangeActivePlatform;

            optionsScrollArea = new GUIScrollArea();
            optionsLayout.AddElement(optionsScrollArea);

            BuildPlatformOptionsGUI();
        }

        private void OnEditorUpdate()
        {
            if (buildScheduled)
            {
                BuildManager.Build();
                ProgressBar.Hide();
                buildScheduled = false;
            }
        }

        /// <summary>
        /// Changes the currently selected platform. Be aware that while platform is selected and you may build for it,
        /// it will not be made the active platform.
        /// </summary>
        /// <param name="type">Platform that was selected or deselected.</param>
        /// <param name="selected">True if the platform was selected, false otherwise.</param>
        private void OnSelectedPlatformChanged(PlatformType type, bool selected)
        {
            if (selected)
            {
                selectedPlatform = type;
                BuildPlatformOptionsGUI();
            }
        }

        /// <summary>
        /// Changes the currently active build platform.
        /// </summary>
        private void ChangeActivePlatform()
        {
            BuildManager.ActivePlatform = selectedPlatform;
        }

        /// <summary>
        /// (Re)creates GUI with platform-specific options.
        /// </summary>
        private void BuildPlatformOptionsGUI()
        {
            optionsScrollArea.Layout.Clear();
            GUILayout layout = optionsScrollArea.Layout;

            PlatformInfo platformInfo = BuildManager.GetPlatformInfo(selectedPlatform);

            GUIResourceField sceneField = new GUIResourceField(typeof(Prefab), new LocEdString("Startup scene"));
            GUIToggleField debugToggle = new GUIToggleField(new LocEdString("Debug"));
            

            GUIToggleField fullscreenField = new GUIToggleField(new LocEdString("Fullscreen"));
            GUIIntField widthField = new GUIIntField(new LocEdString("Window width"));
            GUIIntField heightField = new GUIIntField(new LocEdString("Window height"));

            GUITextField definesField = new GUITextField(new LocEdString("Defines"));

            layout.AddSpace(5);
            layout.AddElement(sceneField);
            layout.AddElement(debugToggle);
            layout.AddElement(fullscreenField);
            layout.AddElement(widthField);
            layout.AddElement(heightField);
            layout.AddSpace(5);
            layout.AddElement(definesField);
            layout.AddSpace(5);

            sceneField.Value = platformInfo.MainScene;
            debugToggle.Value = platformInfo.Debug;
            definesField.Value = platformInfo.Defines;
            fullscreenField.Value = platformInfo.Fullscreen;
            widthField.Value = platformInfo.WindowedWidth;
            heightField.Value = platformInfo.WindowedHeight;

            if (platformInfo.Fullscreen)
            {
                widthField.Active = false;
                heightField.Active = false;
            }

            sceneField.OnChanged += x => platformInfo.MainScene = (Prefab)x;
            debugToggle.OnChanged += x => platformInfo.Debug = x;
            definesField.OnChanged += x => platformInfo.Defines = x;
            fullscreenField.OnChanged += x =>
            {
                widthField.Active = !x;
                heightField.Active = !x;

                platformInfo.Fullscreen = x;
            };
            widthField.OnChanged += x => platformInfo.WindowedWidth = x;
            heightField.OnChanged += x => platformInfo.WindowedHeight = x;

            switch (platformInfo.Type)
            {
                case PlatformType.Windows:
                {
                    WinPlatformInfo winPlatformInfo = (WinPlatformInfo) platformInfo;

                    GUITextField titleField = new GUITextField(new LocEdString("Title"));
                    GUIToggle iconsFoldout = new GUIToggle(new LocEdString("Icons"), EditorStyles.Foldout);

                    layout.AddElement(titleField);
                    layout.AddElement(iconsFoldout);
                    GUILayoutY iconsLayout = layout.AddLayoutY();

                    int[] iconSizes = (int[]) Enum.GetValues(typeof (WinIconSizes));
                    GUITextureField[] iconFields = new GUITextureField[iconSizes.Length];
                    for (int i = 0; i < iconSizes.Length; i++)
                    {
                        int size = iconSizes[i];
                        iconFields[i] = new GUITextureField(new LocEdString("Icon (" + size + "x" + size + ")"));
                        iconFields[i].Value = winPlatformInfo.GetIcon((WinIconSizes) size);
                        iconFields[i].OnChanged += x => winPlatformInfo.SetIcon((WinIconSizes) size, x as Texture2D);

                        iconsLayout.AddElement(iconFields[i]);
                    }

                    GUITextureField taskbarIconField = new GUITextureField(new LocEdString("Taskbar icon (32x32)"));
                    iconsLayout.AddElement(taskbarIconField);

                    titleField.Value = winPlatformInfo.TitleText;
                    taskbarIconField.Value = winPlatformInfo.TaskbarIcon;

                    titleField.OnChanged += x => winPlatformInfo.TitleText = x;
                    iconsFoldout.OnToggled += x => iconsLayout.Active = x;
                    taskbarIconField.OnChanged += x => winPlatformInfo.TaskbarIcon = x as Texture2D;

                    iconsLayout.Active = false;
                }
                    break;
            }

            GUIButton buildButton = new GUIButton(new LocEdString("Build"));
            layout.AddFlexibleSpace();
            layout.AddElement(buildButton);

            buildButton.OnClick += TryStartBuild;

            // TODO - Different background for platform selection bit
        }

        /// <summary>
        /// Starts the build process for the currently selected platform.
        /// </summary>
        private void Build()
        {
            ProgressBar.Show(new LocEdString("Building..."), 0.0f);

            EditorApplication.SaveProject();
            // HACK - Delay build one frame so that progress bar has a chance to show. Use coroutines here once implemented.
            buildScheduled = true;
        }

        /// <summary>
        /// Attempts to save the current scene, and keeps retrying if failed or until user cancels.
        /// </summary>
        private void TrySaveScene()
        {
            EditorApplication.SaveScene(Build, TrySaveScene);
        }

        /// <summary>
        /// Attempts to start the build process if user confirms.
        /// </summary>
        private void TryStartBuild()
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
                Build();
            }
        }
    }
}
