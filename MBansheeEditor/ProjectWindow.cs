using System;
using System.Collections.Generic;
using System.IO;
using BansheeEngine;

namespace BansheeEditor
{
    internal enum ProjectViewType
    {
        Grid64, Grid48, Grid32, List16
    }

    internal sealed class ProjectWindow : EditorWindow
    {
        private struct EntryGUI
        {
            public EntryGUI(GUITexture icon, GUILabel label)
            {
                this.icon = icon;
                this.label = label;
            }

            public GUITexture icon;
            public GUILabel label;
        }

        private const int GRID_ENTRY_SPACING = 15;
        private const int LIST_ENTRY_SPACING = 7;
        private const int MAX_LABEL_HEIGHT = 50;
        private static readonly Color PING_COLOR = Color.BansheeOrange;
        private static readonly Color SELECTION_COLOR = Color.DarkCyan;

        private bool hasContentFocus = false;
        private bool HasContentFocus { get { return HasFocus && hasContentFocus; } } // TODO - This is dummy and never set

        private ProjectViewType viewType = ProjectViewType.Grid32;

        private string currentDirectory = "";
        private List<string> selectionPaths = new List<string>();
        private string pingPath = "";

        private GUIScrollArea contentScrollArea;
        private GUIPanel scrollAreaPanel;

        private GUIButton optionsButton;

        private ContextMenu entryContextMenu;

        private Dictionary<string, EntryGUI> pathToGUIEntry = new Dictionary<string, EntryGUI>();

        // Cut/Copy/Paste
        private List<string> copyPaths = new List<string>();
        private List<string> cutPaths = new List<string>();

        internal ProjectViewType ViewType
        {
            get { return viewType; }
            set { viewType = value; Refresh(); }
        }

        [MenuItem("Windows/Project", ButtonModifier.Ctrl, ButtonCode.P)]
        private static void OpenProjectWindow()
        {
            OpenWindow<ProjectWindow>();
        }

        private void OnInitialize()
        {
            ProjectLibrary.OnEntryAdded += OnEntryChanged;
            ProjectLibrary.OnEntryRemoved += OnEntryChanged;

            GUILayoutY contentLayout = GUI.AddLayoutY();

            GUILayoutX searchBarLayout = contentLayout.AddLayoutX();
            GUITextField searchField = new GUITextField();
            searchField.OnChanged += OnSearchChanged;
            GUIButton clearSearchBtn = new GUIButton("C");
            clearSearchBtn.OnClick += ClearSearch;
            clearSearchBtn.SetWidth(40);
            optionsButton = new GUIButton("O");
            optionsButton.OnClick += OpenOptionsWindow;
            optionsButton.SetWidth(40);
            searchBarLayout.AddElement(searchField);
            searchBarLayout.AddElement(clearSearchBtn);
            searchBarLayout.AddElement(optionsButton);

            // TODO - Add search bar + options button with drop-down
            // TODO - Add directory bar + home button

            contentScrollArea = new GUIScrollArea(GUIOption.FlexibleWidth(), GUIOption.FlexibleHeight());
            contentLayout.AddElement(contentScrollArea);

            entryContextMenu = new ContextMenu();
            entryContextMenu.AddItem("Cut", CutSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.X));
            entryContextMenu.AddItem("Copy", CopySelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.C));
            entryContextMenu.AddItem("Duplicate", DuplicateSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.D));
            entryContextMenu.AddItem("Paste", PasteToSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.V));

            Reset();
        }

        public void Ping(Resource resource)
        {
            pingPath = ProjectLibrary.GetPath(resource);

            Refresh();
            ScrollToEntry(pingPath);
        }

        private void Select(List<string> paths)
        {
            selectionPaths = paths;
            pingPath = "";

            Refresh();
        }

        private void EnterDirectory(string directory)
        {
            currentDirectory = directory;
            pingPath = "";
            selectionPaths.Clear();

            Refresh();
        }

        private void Cut(IEnumerable<string> sourcePaths)
        {
            cutPaths.Clear();
            cutPaths.AddRange(sourcePaths);
            copyPaths.Clear();

            Refresh();
        }

        private void Copy(IEnumerable<string> sourcePaths)
        {
            copyPaths.Clear();
            copyPaths.AddRange(sourcePaths);
            cutPaths.Clear();

            Refresh();
        }

        private void Duplicate(IEnumerable<string> sourcePaths)
        {
            foreach (var source in sourcePaths)
            {
                int idx = 0;
                string destination;
                do
                {
                    destination = source + "_" + idx;
                    idx++;
                } while (!ProjectLibrary.Exists(destination));

                ProjectLibrary.Copy(source, destination);
            }
        }

        private void Paste(string destinationFolder)
        {
            if (copyPaths.Count > 0)
            {
                for (int i = 0; i < copyPaths.Count; i++)
                {
                    string destination = Path.Combine(destinationFolder, Path.GetFileName(copyPaths[i]));

                    ProjectLibrary.Copy(copyPaths[i], destination, true);
                }

                Refresh();
            }
            else if (cutPaths.Count > 0)
            {
                for (int i = 0; i < cutPaths.Count; i++)
                {
                    string destination = Path.Combine(destinationFolder, Path.GetFileName(cutPaths[i]));

                    ProjectLibrary.Move(cutPaths[i], destination, true);
                }

                cutPaths.Clear();
                Refresh();
            }
        }

        private void EditorUpdate()
        {
            if (HasContentFocus)
            {
                if (Input.IsButtonHeld(ButtonCode.LeftControl) || Input.IsButtonHeld(ButtonCode.RightControl))
                {
                    if (Input.IsButtonUp(ButtonCode.C))
                    {
                        CopySelection();
                    }
                    else if (Input.IsButtonUp(ButtonCode.X))
                    {
                        CutSelection();
                    }
                    else if (Input.IsButtonUp(ButtonCode.D))
                    {
                        DuplicateSelection();
                    }
                    else if (Input.IsButtonUp(ButtonCode.V))
                    {
                        PasteToSelection();
                    }
                }
            }

            // TODO - Handle input, drag and drop and whatever else might be needed
            // TODO - Animate ping?
            // TODO - Automatically scroll window when dragging near border?
            // TODO - Drag and drop from Explorer should work to import an asset (i.e. DragAndDropArea)
            // - This should be something that should be enabled per editor window perhaps?
        }

        private void OnEntryChanged(string entry)
        {
            Refresh();
        }

        private void ScrollToEntry(string path)
        {
            Rect2I contentBounds = scrollAreaPanel.Bounds;
            Rect2I scrollAreaBounds = contentScrollArea.ContentBounds;

            EntryGUI entryGUI;
            if (!pathToGUIEntry.TryGetValue(path, out entryGUI))
                return;

            Rect2I entryBounds = entryGUI.icon.Bounds;
            float percent = (entryBounds.x - scrollAreaBounds.height * 0.5f) / contentBounds.height;
            percent = MathEx.Clamp01(percent);
            contentScrollArea.VerticalScroll = percent;
        }

        private SpriteTexture GetIcon(LibraryEntry entry)
        {
            if (entry.Type == LibraryEntryType.Directory)
            {
                return EditorBuiltin.FolderIcon;
            }
            else
            {
                FileEntry fileEntry = (FileEntry)entry;
                switch (fileEntry.ResType)
                {
                    case ResourceType.Font:
                        return EditorBuiltin.FontIcon;
                    case ResourceType.Mesh:
                        return EditorBuiltin.MeshIcon;
                    case ResourceType.Texture:
                        return EditorBuiltin.TextureIcon;
                    case ResourceType.PlainText:
                        return null; // TODO
                    case ResourceType.ScriptCode:
                        return null; // TODO
                    case ResourceType.SpriteTexture:
                        return null; // TODO
                    case ResourceType.Shader:
                        return null; // TODO
                    case ResourceType.Material:
                        return null; // TODO
                }
            }

            return null;
        }

        private void Refresh()
        {
            DirectoryEntry entry = ProjectLibrary.GetEntry(currentDirectory) as DirectoryEntry;
            if (entry == null)
            {
                Reset();
                return;
            }

            if (scrollAreaPanel != null)
                scrollAreaPanel.Destroy();

            pathToGUIEntry.Clear();
            scrollAreaPanel = contentScrollArea.Layout.AddPanel();

            GUIPanel contentPanel = scrollAreaPanel.AddPanel(1);
            GUIPanel contentOverlayPanel = scrollAreaPanel.AddPanel(0);
            GUIPanel contentUnderlayPanel = scrollAreaPanel.AddPanel(2);

            GUILayout contentLayout = contentPanel.AddLayoutY();

            Rect2I scrollBounds = contentScrollArea.Bounds;
            LibraryEntry[] childEntries = entry.Children;

            if (childEntries.Length == 0)
                return;

            if (viewType == ProjectViewType.List16)
            {
                int tileSize = 16;

                for (int i = 0; i < childEntries.Length; i++)
                {
                    LibraryEntry currentEntry = childEntries[i];

                    CreateEntryGUI(contentLayout, tileSize, false, currentEntry);

                    if (i != childEntries.Length - 1)
                        contentLayout.AddSpace(LIST_ENTRY_SPACING);
                }

                contentLayout.AddFlexibleSpace();
            }
            else
            {
                int tileSize = 64;
                switch (viewType)
                {
                    case ProjectViewType.Grid64: tileSize = 64; break;
                    case ProjectViewType.Grid48: tileSize = 48; break;
                    case ProjectViewType.Grid32: tileSize = 32; break;
                }

                GUILayoutX rowLayout = contentLayout.AddLayoutX();

                rowLayout.AddFlexibleSpace();
                int currentWidth = GRID_ENTRY_SPACING * 2;
                bool addedAny = false;

                for (int i = 0; i < childEntries.Length; i++)
                {
                    if (currentWidth >= scrollBounds.width && addedAny) // We force at least one entry per row, even if it doesn't fit
                    {
                        rowLayout = contentLayout.AddLayoutX();
                        contentLayout.AddFlexibleSpace();

                        rowLayout.AddFlexibleSpace();

                        currentWidth = GRID_ENTRY_SPACING * 2;
                    }

                    LibraryEntry currentEntry = childEntries[i];
                    CreateEntryGUI(rowLayout, tileSize, true, currentEntry);
                    rowLayout.AddFlexibleSpace();

                    addedAny = true;
                    currentWidth += tileSize + GRID_ENTRY_SPACING;
                }
            }

            for (int i = 0; i < childEntries.Length; i++)
            {
                LibraryEntry currentEntry = childEntries[i];
                CreateEntryOverlayGUI(contentOverlayPanel, contentUnderlayPanel, pathToGUIEntry[currentEntry.Path], currentEntry);
            }

            Rect2I contentBounds = contentLayout.Bounds;
            GUIButton catchAll = new GUIButton("", EditorStyles.Blank);
            catchAll.Bounds = contentBounds;
            catchAll.OnClick += OnCatchAllClicked;
            catchAll.SetContextMenu(entryContextMenu);

            contentUnderlayPanel.AddElement(catchAll);

            Debug.Log("REFRESHED " + Time.FrameNumber);
        }

        private void CreateEntryGUI(GUILayout parentLayout, int tileSize, bool grid, LibraryEntry entry)
        {
            GUILayout entryLayout;
            
            if(grid)
                entryLayout = parentLayout.AddLayoutY();
            else
                entryLayout = parentLayout.AddLayoutX();

            SpriteTexture iconTexture = GetIcon(entry);

            GUITexture icon = new GUITexture(iconTexture, GUIImageScaleMode.ScaleToFit,
                true, GUIOption.FixedHeight(tileSize), GUIOption.FixedWidth(tileSize));

            GUILabel label = null;

            if (grid)
            {
                label = new GUILabel(entry.Name, EditorStyles.MultiLineLabel,
                    GUIOption.FixedWidth(tileSize), GUIOption.FlexibleHeight(0, MAX_LABEL_HEIGHT));
            }
            else
            {
                label = new GUILabel(entry.Name);   
            }

            entryLayout.AddElement(icon);
            entryLayout.AddElement(label);

            pathToGUIEntry[entry.Path] = new EntryGUI(icon, label);
        }

        private void CreateEntryOverlayGUI(GUIPanel overlayPanel, GUIPanel underlayPanel, EntryGUI gui, LibraryEntry entry)
        {
            // Add overlay button
            Rect2I entryButtonBounds = gui.icon.Bounds;
            Rect2I labelBounds = gui.label.Bounds;

            entryButtonBounds.x = MathEx.Min(entryButtonBounds.x, labelBounds.x);
            entryButtonBounds.y = MathEx.Min(entryButtonBounds.y, labelBounds.y);
            entryButtonBounds.width = MathEx.Max(entryButtonBounds.x + entryButtonBounds.width,
                labelBounds.x + labelBounds.width) - entryButtonBounds.x;
            entryButtonBounds.height = MathEx.Max(entryButtonBounds.y + entryButtonBounds.height,
                labelBounds.y + labelBounds.height) - entryButtonBounds.y;

            GUIButton overlayBtn = new GUIButton("", EditorStyles.Blank);
            overlayBtn.Bounds = entryButtonBounds;
            overlayBtn.OnClick += () => OnEntryClicked(entry.Path);
            overlayBtn.OnDoubleClick += () => OnEntryDoubleClicked(entry.Path);
            overlayBtn.SetContextMenu(entryContextMenu);

            overlayPanel.AddElement(overlayBtn);

            if (cutPaths.Contains(entry.Path))
            {
                gui.icon.SetTint(new Color(1.0f, 1.0f, 1.0f, 0.5f));
            }

            if (selectionPaths.Contains(entry.Path))
            {
                GUITexture underlay = new GUITexture(Builtin.WhiteTexture);
                underlay.Bounds = entryButtonBounds;
                underlay.SetTint(SELECTION_COLOR);

                underlayPanel.AddElement(underlay);
            }
            else if (pingPath == entry.Path)
            {
                GUITexture underlay = new GUITexture(Builtin.WhiteTexture);
                underlay.Bounds = entryButtonBounds;
                underlay.SetTint(PING_COLOR);

                underlayPanel.AddElement(underlay);
            }
        }

        private void OnEntryClicked(string path)
        {
            Select(new List<string> { path });

            Selection.resourcePaths = new string[] {path};

            Debug.Log("CLICKED " + Time.FrameNumber);
        }

        private void OnEntryDoubleClicked(string path)
        {
            Debug.Log("DOUBLE CLICK " + path);

            LibraryEntry entry = ProjectLibrary.GetEntry(path);
            if (entry != null && entry.Type == LibraryEntryType.Directory)
            {
                EnterDirectory(path);
            }
        }

        private void OnCatchAllClicked()
        {
            Select(new List<string> { });

            Selection.resourcePaths = new string[] { };
        }

        private void CutSelection()
        {
            if (selectionPaths.Count > 0)
                Cut(selectionPaths);
        }

        private void CopySelection()
        {
            if (selectionPaths.Count > 0)
                Copy(selectionPaths);
        }

        private void DuplicateSelection()
        {
            if (selectionPaths.Count > 0)
                Duplicate(selectionPaths);
        }

        private void PasteToSelection()
        {
            DirectoryEntry selectedDirectory = null;
            if (selectionPaths.Count == 1)
            {
                LibraryEntry entry = ProjectLibrary.GetEntry(selectionPaths[0]);
                if (entry != null && entry.Type == LibraryEntryType.Directory)
                    selectedDirectory = (DirectoryEntry) entry;
            }

            if(selectedDirectory != null)
                Paste(selectedDirectory.Path);
            else
                Paste(currentDirectory);
        }

        private void OnSearchChanged(string newValue)
        {
            // TODO
        }

        private void ClearSearch()
        {
            // TODO
        }

        private void OpenOptionsWindow()
        {
            Vector2I openPosition;
            Rect2I buttonBounds = GUILayoutUtility.CalculateBounds(optionsButton, GUI);

            openPosition.x = buttonBounds.x + buttonBounds.width / 2;
            openPosition.y = buttonBounds.y + buttonBounds.height / 2;

            ProjectDropDown dropDown = DropDownWindow.Open<ProjectDropDown>(this, openPosition);
            dropDown.SetParent(this);
        }

        private void Reset()
        {
            currentDirectory = ProjectLibrary.Root.Path;
            Refresh();
        }

        protected override void WindowResized(int width, int height)
        {
            base.WindowResized(width, height);

            Refresh();
        }
    }

    internal class ProjectDropDown : DropDownWindow
    {
        private ProjectWindow parent;

        public ProjectDropDown()
            :base(100, 50)
        { }

        internal void SetParent(ProjectWindow parent)
        {
            this.parent = parent;

            GUIToggleGroup group = new GUIToggleGroup();

            GUIToggle list16 = new GUIToggle("16", group, EditorStyles.Button, GUIOption.FixedWidth(30));
            GUIToggle grid32 = new GUIToggle("32", group, EditorStyles.Button, GUIOption.FixedWidth(30));
            GUIToggle grid48 = new GUIToggle("48", group, EditorStyles.Button, GUIOption.FixedWidth(30));
            GUIToggle grid64 = new GUIToggle("64", group, EditorStyles.Button, GUIOption.FixedWidth(30));

            ProjectViewType activeType = parent.ViewType;
            switch (activeType)
            {
                case ProjectViewType.List16:
                    list16.ToggleOn();
                    break;
                case ProjectViewType.Grid32:
                    grid32.ToggleOn();
                    break;
                case ProjectViewType.Grid48:
                    grid48.ToggleOn();
                    break;
                case ProjectViewType.Grid64:
                    grid64.ToggleOn();
                    break;
            }

            list16.OnToggled += (active) =>
            {
                if (active)
                    ChangeViewType(ProjectViewType.List16);
            };

            grid32.OnToggled += (active) =>
            {
                if (active)
                    ChangeViewType(ProjectViewType.Grid32);
            };

            grid48.OnToggled += (active) =>
            {
                if (active)
                    ChangeViewType(ProjectViewType.Grid48);
            };

            grid64.OnToggled += (active) =>
            {
                if (active)
                    ChangeViewType(ProjectViewType.Grid64);
            };

            GUILayoutY vertLayout = GUI.AddLayoutY();

            vertLayout.AddFlexibleSpace();
            GUILayoutX contentLayout = vertLayout.AddLayoutX();
            contentLayout.AddFlexibleSpace();
            contentLayout.AddElement(list16);
            contentLayout.AddElement(grid32);
            contentLayout.AddElement(grid48);
            contentLayout.AddElement(grid64);
            contentLayout.AddFlexibleSpace();
            vertLayout.AddFlexibleSpace();
        }

        private void ChangeViewType(ProjectViewType viewType)
        {
            parent.ViewType = viewType;
        }
    }
}
