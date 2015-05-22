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
        private class ContentInfo
        {
            public ContentInfo(ProjectWindow window, ProjectViewType viewType)
            {
                GUIPanel parentPanel = window.scrollAreaPanel;

                GUIPanel contentPanel = parentPanel.AddPanel(1);
                overlay = parentPanel.AddPanel(0);
                underlay = parentPanel.AddPanel(2);

                main = contentPanel.AddLayoutY();

                if (viewType == ProjectViewType.List16)
                {
                    tileSize = 16;
                    gridLayout = false;
                }
                else
                {
                    switch (viewType)
                    {
                        case ProjectViewType.Grid64:
                            tileSize = 64;
                            break;
                        case ProjectViewType.Grid48:
                            tileSize = 48;
                            break;
                        case ProjectViewType.Grid32:
                            tileSize = 32;
                            break;
                    }

                    gridLayout = true;
                }

                this.window = window;
            }

            public GUILayout main;
            public GUIPanel overlay;
            public GUIPanel underlay;

            public ProjectWindow window;
            public int tileSize;
            public bool gridLayout;
        }

        private class ElementEntry
        {
            // Note: Order of these is relevant
            enum UnderlayState
            {
                None, Hovered, Selected, Pinged
            }

            public string path;
            public GUITexture icon;
            public GUILabel label;
            public Rect2I bounds;

            private GUITexture underlay;
            private ContentInfo info;
            private UnderlayState underlayState;

            public ElementEntry(ContentInfo info, GUILayout parent, LibraryEntry entry)
            {
                GUILayout entryLayout;

                if (info.gridLayout)
                    entryLayout = parent.AddLayoutY();
                else
                    entryLayout = parent.AddLayoutX();

                SpriteTexture iconTexture = GetIcon(entry);

                icon = new GUITexture(iconTexture, GUIImageScaleMode.ScaleToFit,
                    true, GUIOption.FixedHeight(info.tileSize), GUIOption.FixedWidth(info.tileSize));

                label = null;

                if (info.gridLayout)
                {
                    label = new GUILabel(entry.Name, EditorStyles.MultiLineLabel,
                        GUIOption.FixedWidth(info.tileSize), GUIOption.FlexibleHeight(0, MAX_LABEL_HEIGHT));
                }
                else
                {
                    label = new GUILabel(entry.Name);
                }

                entryLayout.AddElement(icon);
                entryLayout.AddElement(label);

                this.info = info;
                this.path = entry.Path;
                this.bounds = new Rect2I();
                this.underlay = null;
            }

            public void Initialize()
            {
                bounds = icon.Bounds;
                Rect2I labelBounds = label.Bounds;

                bounds.x = MathEx.Min(bounds.x, labelBounds.x);
                bounds.y = MathEx.Min(bounds.y, labelBounds.y);
                bounds.width = MathEx.Max(bounds.x + bounds.width,
                    labelBounds.x + labelBounds.width) - bounds.x;
                bounds.height = MathEx.Max(bounds.y + bounds.height,
                    labelBounds.y + labelBounds.height) - bounds.y;

                ProjectWindow hoistedWindow = info.window;
                string hoistedPath = path;

                GUIButton overlayBtn = new GUIButton("", EditorStyles.Blank);
                overlayBtn.Bounds = bounds;
                overlayBtn.OnClick += () => hoistedWindow.OnEntryClicked(hoistedPath);
                overlayBtn.OnDoubleClick += () => hoistedWindow.OnEntryDoubleClicked(hoistedPath);
                overlayBtn.SetContextMenu(info.window.entryContextMenu);

                info.overlay.AddElement(overlayBtn);
            }

            public Rect2I Bounds
            {
                get { return bounds; }
            }

            public void MarkAsCut(bool enable)
            {
                if (enable)
                    icon.SetTint(CUT_COLOR);
                else
                    icon.SetTint(Color.White);
            }

            public void MarkAsSelected(bool enable)
            {
                if ((int)underlayState > (int) UnderlayState.Selected)
                    return;

                if (enable)
                {
                    CreateUnderlay();
                    underlay.SetTint(SELECTION_COLOR);
                }
                else
                    ClearUnderlay();

                underlayState = UnderlayState.Selected;
            }

            public void MarkAsPinged(bool enable)
            {
                if ((int)underlayState > (int)UnderlayState.Pinged)
                    return;

                if (enable)
                {
                    CreateUnderlay();
                    underlay.SetTint(PING_COLOR);
                }
                else
                    ClearUnderlay();

                underlayState = UnderlayState.Pinged;
            }

            public void MarkAsHovered(bool enable)
            {
                if ((int)underlayState > (int)UnderlayState.Hovered)
                    return;

                if (enable)
                {
                    CreateUnderlay();
                    underlay.SetTint(HOVER_COLOR);
                }
                else
                    ClearUnderlay();

                underlayState = UnderlayState.Hovered;
            }

            private void ClearUnderlay()
            {
                if (underlay != null)
                {
                    underlay.Destroy();
                    underlay = null;
                }

                underlayState = UnderlayState.None;
            }

            private void CreateUnderlay()
            {
                if (underlay == null)
                {
                    underlay = new GUITexture(Builtin.WhiteTexture);
                    underlay.Bounds = Bounds;

                    info.underlay.AddElement(underlay);
                }
            }

            private static SpriteTexture GetIcon(LibraryEntry entry)
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
        }

        private const int GRID_ENTRY_SPACING = 15;
        private const int LIST_ENTRY_SPACING = 7;
        private const int MAX_LABEL_HEIGHT = 50;
        private const int DRAG_SCROLL_HEIGHT = 20;
        private const int DRAG_SCROLL_AMOUNT_PER_SECOND = 100;
        private static readonly Color PING_COLOR = Color.BansheeOrange;
        private static readonly Color SELECTION_COLOR = Color.DarkCyan;
        private static readonly Color HOVER_COLOR = new Color(Color.DarkCyan.r, Color.DarkCyan.g, Color.DarkCyan.b, 0.5f);
        private static readonly Color CUT_COLOR = new Color(1.0f, 1.0f, 1.0f, 0.5f);

        private bool hasContentFocus = false;
        private bool HasContentFocus { get { return HasFocus && hasContentFocus; } }

        private ProjectViewType viewType = ProjectViewType.Grid32;

        private string currentDirectory = "";
        private List<string> selectionPaths = new List<string>();
        private string pingPath = "";
        private string hoverHighlightPath = "";

        private GUIScrollArea contentScrollArea;
        private GUIPanel scrollAreaPanel;
        private GUILayoutX searchBarLayout;
        private GUIButton optionsButton;

        private ContextMenu entryContextMenu;
        private ProjectDropTarget dropTarget;

        private List<ElementEntry> entries = new List<ElementEntry>();
        private Dictionary<string, ElementEntry> entryLookup = new Dictionary<string, ElementEntry>(); 

        private int autoScrollAmount;

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

            searchBarLayout = contentLayout.AddLayoutX();
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
            contentLayout.AddFlexibleSpace();

            entryContextMenu = new ContextMenu();
            entryContextMenu.AddItem("Cut", CutSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.X));
            entryContextMenu.AddItem("Copy", CopySelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.C));
            entryContextMenu.AddItem("Duplicate", DuplicateSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.D));
            entryContextMenu.AddItem("Paste", PasteToSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.V));

            Reset();

            dropTarget = new ProjectDropTarget(this);
            dropTarget.Bounds = contentScrollArea.Bounds;
            dropTarget.OnStart += DoOnDragStart;
            dropTarget.OnDrag += DoOnDragMove;
            dropTarget.OnLeave += DoOnDragLeave;
            dropTarget.OnDrop += DoOnDragDropped;
        }

        private ElementEntry FindElementAt(Vector2I windowPos)
        {
            Rect2I scrollBounds = contentScrollArea.Layout.Bounds;
            Vector2I scrollPos = windowPos;
            scrollPos.x -= scrollBounds.x;
            scrollPos.y -= scrollBounds.y;

            foreach (var element in entries)
            {
                if (element.bounds.Contains(scrollPos))
                    return element;
            }

            return null;
        }

        private void DoOnDragStart(Vector2I windowPos)
        {
            ElementEntry underCursorElem = FindElementAt(windowPos);
            if (underCursorElem == null)
                return;

            if (!selectionPaths.Contains(underCursorElem.path))
                Select(new List<string> { underCursorElem.path });

            ResourceDragDropData dragDropData = new ResourceDragDropData(selectionPaths.ToArray());
            DragDrop.StartDrag(dragDropData);
        }

        private void DoOnDragMove(Vector2I windowPos)
        {
            ElementEntry underCursorElem = FindElementAt(windowPos);
            
            if (underCursorElem == null)
            {
                ClearHoverHighlight();
            }
            else
            {
                if (underCursorElem.path != hoverHighlightPath)
                {
                    ClearHoverHighlight();

                    hoverHighlightPath = underCursorElem.path;
                    underCursorElem.MarkAsHovered(true);
                }
            }

            Rect2I scrollAreaBounds = contentScrollArea.Bounds;
            int scrollAreaTop = scrollAreaBounds.y;
            int scrollAreaBottom = scrollAreaBounds.y + scrollAreaBounds.height;

            if (windowPos.y > scrollAreaTop && windowPos.y <= (scrollAreaTop + DRAG_SCROLL_HEIGHT))
                autoScrollAmount = -DRAG_SCROLL_AMOUNT_PER_SECOND;
            else if (windowPos.y >= (scrollAreaBottom - DRAG_SCROLL_HEIGHT) && windowPos.y < scrollAreaBottom)
                autoScrollAmount = DRAG_SCROLL_AMOUNT_PER_SECOND;
            else
                autoScrollAmount = 0;
        }

        private void DoOnDragLeave()
        {
            ClearHoverHighlight();
            autoScrollAmount = 0;
        }

        private void DoOnDragDropped(Vector2I windowPos, string[] paths)
        {
            string resourceDir = ProjectLibrary.ResourceFolder;
            string destinationFolder = Path.Combine(resourceDir, currentDirectory);

            ElementEntry underCursorElement = FindElementAt(windowPos);
            if (underCursorElement != null)
            {
                LibraryEntry entry = ProjectLibrary.GetEntry(underCursorElement.path);
                if (entry != null && entry.Type == LibraryEntryType.Directory)
                    destinationFolder = Path.Combine(resourceDir, entry.Path);
            }

            if (paths != null)
            {
                foreach (var path in paths)
                {
                    if (path == null)
                        continue;

                    string absolutePath = path;
                    if (!Path.IsPathRooted(absolutePath))
                        absolutePath = Path.Combine(resourceDir, path);

                    if (string.IsNullOrEmpty(absolutePath))
                        continue;

                    if (PathEx.IsPartOf(destinationFolder, absolutePath) || PathEx.Compare(absolutePath, destinationFolder))
                        continue;

                    string destination = Path.Combine(destinationFolder, Path.GetFileName(absolutePath));

                    if (ProjectLibrary.Exists(path))
                        ProjectLibrary.Move(path, destination, true);
                    else
                        ProjectLibrary.Copy(path, destination, true);
                }
            }

            ClearHoverHighlight();
            autoScrollAmount = 0;
        }

        private void ClearHoverHighlight()
        {
            if (!string.IsNullOrEmpty(hoverHighlightPath))
            {
                ElementEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(hoverHighlightPath, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsHovered(false);
            }

            hoverHighlightPath = "";
        }

        public void Ping(Resource resource)
        {
            if (!string.IsNullOrEmpty(pingPath))
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(pingPath, out entry))
                    entry.MarkAsPinged(false);
            }

            if (resource != null)
                pingPath = ProjectLibrary.GetPath(resource);
            else
                pingPath = "";

            if (!string.IsNullOrEmpty(pingPath))
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(pingPath, out entry))
                    entry.MarkAsPinged(true);

                ScrollToEntry(pingPath);
            }
        }

        private void Select(List<string> paths)
        {
            if (selectionPaths != null)
            {
                foreach (var path in selectionPaths)
                {
                    ElementEntry entry;
                    if (entryLookup.TryGetValue(path, out entry))
                        entry.MarkAsSelected(false);
                }
            }

            selectionPaths = paths;

            if (selectionPaths != null)
            {
                foreach (var path in selectionPaths)
                {
                    ElementEntry entry;
                    if (entryLookup.TryGetValue(path, out entry))
                        entry.MarkAsSelected(true);
                }
            }

            Ping(null);
        }

        private void EnterDirectory(string directory)
        {
            currentDirectory = directory;
            Ping(null);
            Select(new List<string>());

            Refresh();
        }

        private void Cut(IEnumerable<string> sourcePaths)
        {
            foreach (var path in cutPaths)
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(path, out entry))
                    entry.MarkAsCut(false);
            }

            cutPaths.Clear();
            cutPaths.AddRange(sourcePaths);

            foreach (var path in cutPaths)
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(path, out entry))
                    entry.MarkAsCut(true);
            }

            copyPaths.Clear();
        }

        private void Copy(IEnumerable<string> sourcePaths)
        {
            copyPaths.Clear();
            copyPaths.AddRange(sourcePaths);

            foreach (var path in cutPaths)
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(path, out entry))
                    entry.MarkAsCut(false);
            }

            cutPaths.Clear();
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

            if (autoScrollAmount != 0)
            {
                Rect2I contentBounds = contentScrollArea.ContentBounds;
                float scrollPct = autoScrollAmount / (float)contentBounds.height;

                contentScrollArea.VerticalScroll += scrollPct * Time.FrameDelta;
            }

            dropTarget.Update();
        }

        private void OnEntryChanged(string entry)
        {
            Refresh();
        }

        private void ScrollToEntry(string path)
        {
            Rect2I contentBounds = scrollAreaPanel.Bounds;
            Rect2I scrollAreaBounds = contentScrollArea.ContentBounds;

            ElementEntry entryGUI;
            if (!entryLookup.TryGetValue(path, out entryGUI))
                return;

            Rect2I entryBounds = entryGUI.icon.Bounds;
            float percent = (entryBounds.x - scrollAreaBounds.height * 0.5f) / contentBounds.height;
            percent = MathEx.Clamp01(percent);
            contentScrollArea.VerticalScroll = percent;
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

            entries.Clear();
            entryLookup.Clear();
            scrollAreaPanel = contentScrollArea.Layout.AddPanel();

            ContentInfo contentInfo = new ContentInfo(this, viewType);

            Rect2I scrollBounds = contentScrollArea.Bounds;
            LibraryEntry[] childEntries = entry.Children;

            if (childEntries.Length == 0)
                return;

            if (viewType == ProjectViewType.List16)
            {
                for (int i = 0; i < childEntries.Length; i++)
                {
                    ElementEntry guiEntry = new ElementEntry(contentInfo, contentInfo.main, childEntries[i]);
                    entries.Add(guiEntry);
                    entryLookup[guiEntry.path] = guiEntry;

                    if (i != childEntries.Length - 1)
                        contentInfo.main.AddSpace(LIST_ENTRY_SPACING);
                }

                contentInfo.main.AddFlexibleSpace();
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

                GUILayoutX rowLayout = contentInfo.main.AddLayoutX();
                rowLayout.AddFlexibleSpace();

                int elemSize = tileSize + GRID_ENTRY_SPACING;
                int elemsPerRow = (scrollBounds.width - GRID_ENTRY_SPACING*2)/elemSize;

                int elemsInRow = 0;

                for (int i = 0; i < childEntries.Length; i++)
                {
                    if (elemsInRow == elemsPerRow && elemsInRow > 0)
                    {
                        rowLayout = contentInfo.main.AddLayoutX();
                        contentInfo.main.AddSpace(GRID_ENTRY_SPACING);

                        rowLayout.AddFlexibleSpace();
                        elemsInRow = 0;
                    }

                    ElementEntry guiEntry = new ElementEntry(contentInfo, rowLayout, childEntries[i]);
                    entries.Add(guiEntry);
                    entryLookup[guiEntry.path] = guiEntry;

                    rowLayout.AddFlexibleSpace();

                    elemsInRow++;
                }

                int extraElements = elemsPerRow - elemsInRow;
                for (int i = 0; i < extraElements; i++)
                {
                    rowLayout.AddSpace(tileSize);
                    rowLayout.AddFlexibleSpace();
                }

                contentInfo.main.AddFlexibleSpace();
            }

            for (int i = 0; i < entries.Count; i++)
            {
                ElementEntry guiEntry = entries[i];
                guiEntry.Initialize();

                if (cutPaths.Contains(guiEntry.path))
                    guiEntry.MarkAsCut(true);

                if (selectionPaths.Contains(guiEntry.path))
                    guiEntry.MarkAsSelected(true);
                else if (pingPath == guiEntry.path)
                    guiEntry.MarkAsPinged(true);
            }

            Rect2I contentBounds = contentInfo.main.Bounds;
            Rect2I minimalBounds = GetScrollAreaBounds();
            contentBounds.width = Math.Max(contentBounds.width, minimalBounds.width);
            contentBounds.height = Math.Max(contentBounds.height, minimalBounds.height);

            Debug.Log(contentBounds + " - " + minimalBounds);

            GUIButton catchAll = new GUIButton("", EditorStyles.Blank);
            catchAll.Bounds = contentBounds;
            catchAll.OnClick += OnCatchAllClicked;
            catchAll.SetContextMenu(entryContextMenu);
            catchAll.OnFocusChanged += OnContentsFocusChanged;

            contentInfo.underlay.AddElement(catchAll);
        }

        private void OnContentsFocusChanged(bool focus)
        {
            if(focus)
                Debug.Log("GOT FOCUS");
            else
                Debug.Log("LOST FOCUS");

            hasContentFocus = focus;
        }

        private void OnEntryClicked(string path)
        {
            Select(new List<string> { path });

            Selection.resourcePaths = new string[] {path};
        }

        private void OnEntryDoubleClicked(string path)
        {
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

        private Rect2I GetScrollAreaBounds()
        {
            Rect2I bounds = GUI.Bounds;
            Rect2I searchBarBounds = searchBarLayout.Bounds;
            bounds.y += searchBarBounds.height;
            bounds.height -= searchBarBounds.height;

            return bounds;
        }

        protected override void WindowResized(int width, int height)
        {
            base.WindowResized(width, height);

            Refresh();

            dropTarget.Bounds = contentScrollArea.Bounds;
        }
    }

    internal class ProjectDropDown : DropDownWindow
    {
        private ProjectWindow parent;

        public ProjectDropDown()
            :base(150, 30)
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
