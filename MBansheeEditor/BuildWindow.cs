using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Provides options for customizing and activating the build process which will output an executable of the game for a 
    /// specific platform, as well as any required resources.
    /// </summary>
    [DefaultSize(500, 300)]
    internal sealed class BuildWindow : EditorWindow
    {
        private static readonly Color PLATFORM_BG_COLOR = new Color(33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f);

        private PlatformType selectedPlatform;
        private GUIScrollArea optionsScrollArea;
        private ulong buildScheduledFrame = ulong.MaxValue;

        private GUIToggle[] platformButtons;

        /// <summary>
        /// Opens the build window if its not open already.
        /// </summary>
        [MenuItem("Tools/Build", ButtonModifier.CtrlAlt, ButtonCode.B, 9296)]
        private static void OpenBuildWindow()
        {
            OpenWindow<BuildWindow>();
        }

        /// <inheritdoc/>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Build");
        }

        private void OnInitialize()
        {
            GUILayoutX splitLayout = GUI.AddLayoutX();
            GUIPanel platformPanel = splitLayout.AddPanel();
            GUIPanel platformForeground = platformPanel.AddPanel();
            GUILayoutY platformLayout = platformForeground.AddLayoutY();
            GUIPanel platformBackground = platformPanel.AddPanel(1);
            GUITexture background = new GUITexture(Builtin.WhiteTexture);
            background.SetTint(PLATFORM_BG_COLOR);

            splitLayout.AddSpace(5);
            GUILayoutY optionsLayout = splitLayout.AddLayoutY();

            GUILabel platformsLabel = new GUILabel(new LocEdString("Platforms"), EditorStyles.LabelCentered);
            platformLayout.AddSpace(5);
            platformLayout.AddElement(platformsLabel);
            platformLayout.AddSpace(5);

            GUIToggleGroup platformToggleGroup = new GUIToggleGroup();

            PlatformType[] availablePlatforms = BuildManager.AvailablePlatforms;
            platformButtons = new GUIToggle[availablePlatforms.Length];
            for (int i = 0; i < availablePlatforms.Length; i++)
            {
                PlatformType currentPlatform = availablePlatforms[i];
                bool isActive = currentPlatform == BuildManager.ActivePlatform;

                string platformName = Enum.GetName(typeof(PlatformType), currentPlatform);
                if (isActive)
                    platformName += " (Active)";

                GUIToggle platformToggle = new GUIToggle(new LocEdString(platformName), platformToggleGroup, EditorStyles.Button);
                platformToggle.OnToggled += x => OnSelectedPlatformChanged(currentPlatform, x);
                platformLayout.AddElement(platformToggle);

                platformButtons[i] = platformToggle;

                if (isActive)
                {
                    platformToggle.Value = true;
                    selectedPlatform = currentPlatform;
                }
            }

            platformLayout.AddFlexibleSpace();

            GUIButton changePlatformBtn = new GUIButton(new LocEdString("Set active"));
            platformLayout.AddElement(changePlatformBtn);
            changePlatformBtn.OnClick += ChangeActivePlatform;

            platformBackground.AddElement(background);

            optionsScrollArea = new GUIScrollArea();
            optionsLayout.AddElement(optionsScrollArea);

            GUIButton buildButton = new GUIButton(new LocEdString("Build"));
            optionsLayout.AddFlexibleSpace();
            optionsLayout.AddElement(buildButton);

            buildButton.OnClick += TryStartBuild;

            BuildPlatformOptionsGUI();
        }

        private void OnEditorUpdate()
        {
            if (buildScheduledFrame == Time.FrameIdx)
            {
                BuildManager.Build();
                ProgressBar.Hide();

                EditorApplication.OpenExternally(BuildManager.OutputFolder);
                DialogBox.Open(new LocEdString("Build complete"), new LocEdString("Build complete"), DialogBox.Type.OK);
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

            PlatformType[] availablePlatforms = BuildManager.AvailablePlatforms;
            for (int i = 0; i < availablePlatforms.Length; i++)
            {
                PlatformType currentPlatform = availablePlatforms[i];
                bool isActive = currentPlatform == BuildManager.ActivePlatform;

                string platformName = Enum.GetName(typeof (PlatformType), currentPlatform);
                if (isActive)
                    platformName += " (Active)";

                platformButtons[i].SetContent(new LocEdString(platformName));
            }
        }

        /// <summary>
        /// (Re)creates GUI with platform-specific options.
        /// </summary>
        private void BuildPlatformOptionsGUI()
        {
            optionsScrollArea.Layout.Clear();
            GUILayout layout = optionsScrollArea.Layout;

            PlatformInfo platformInfo = BuildManager.GetPlatformInfo(selectedPlatform);

            GUILabel options = new GUILabel(new LocEdString("Platform options"), EditorStyles.LabelCentered);

            GUIResourceField sceneField = new GUIResourceField(typeof(Prefab), new LocEdString("Startup scene"));
            GUIToggleField debugToggle = new GUIToggleField(new LocEdString("Debug"));
            
            GUIToggleField fullscreenField = new GUIToggleField(new LocEdString("Fullscreen"));
            GUIIntField widthField = new GUIIntField(new LocEdString("Window width"));
            GUIIntField heightField = new GUIIntField(new LocEdString("Window height"));

            GUITextField definesField = new GUITextField(new LocEdString("Defines"));

            layout.AddSpace(5);
            layout.AddElement(options);
            layout.AddSpace(5);
            layout.AddElement(sceneField);
            layout.AddElement(debugToggle);
            layout.AddElement(fullscreenField);
            layout.AddElement(widthField);
            layout.AddElement(heightField);
            layout.AddSpace(5);
            layout.AddElement(definesField);
            layout.AddSpace(5);

            sceneField.ValueRef = platformInfo.MainScene;
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

            sceneField.OnChanged += x => platformInfo.MainScene = x;
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

                    layout.AddElement(titleField);
                    layout.AddSpace(5);

                    GUITextureField iconField = new GUITextureField(new LocEdString("Icon"));
                    layout.AddElement(iconField);

                    titleField.Value = winPlatformInfo.TitleText;
                    iconField.ValueRef = winPlatformInfo.Icon;

                    titleField.OnChanged += x => winPlatformInfo.TitleText = x;
                    iconField.OnChanged += x => winPlatformInfo.Icon = x;
                }
                    break;
            }
        }

        /// <summary>
        /// Starts the build process for the currently selected platform.
        /// </summary>
        private void Build()
        {
            ProgressBar.Show(new LocEdString("Building..."), 0.0f);

            EditorApplication.SaveProject();
            // HACK - Delay build one frame so that progress bar has a chance to show. Use coroutines here once implemented.
            buildScheduledFrame = Time.FrameIdx + 1;
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
