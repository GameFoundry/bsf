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

    internal sealed class LibraryWindow : EditorWindow
    {
        private const int GRID_ENTRY_SPACING = 15;
        private const int LIST_ENTRY_SPACING = 7;
        private const int MAX_LABEL_HEIGHT = 50;
        private const int MIN_HORZ_SPACING = 8;
        private const int DRAG_SCROLL_HEIGHT = 20;
        private const int DRAG_SCROLL_AMOUNT_PER_SECOND = 100;
        private const int FOLDER_BUTTON_WIDTH = 20;
        private const int FOLDER_SEPARATOR_WIDTH = 7;
        private const int SELECTION_EXTRA_WIDTH = 3;
        private static readonly Color PING_COLOR = Color.BansheeOrange;
        private static readonly Color SELECTION_COLOR = Color.DarkCyan;
        private static readonly Color HOVER_COLOR = new Color(Color.DarkCyan.r, Color.DarkCyan.g, Color.DarkCyan.b, 0.5f);
        private static readonly Color CUT_COLOR = new Color(1.0f, 1.0f, 1.0f, 0.5f);

        private bool hasContentFocus = false;
        private bool HasContentFocus { get { return HasFocus && hasContentFocus; } }

        private string searchQuery;
        private bool IsSearchActive { get { return !string.IsNullOrEmpty(searchQuery); } }

        private ProjectViewType viewType = ProjectViewType.Grid32;

        private bool requiresRefresh;
        private string currentDirectory = "";
        private List<string> selectionPaths = new List<string>();
        private int selectionAnchorStart = -1;
        private int selectionAnchorEnd = -1;
        private string pingPath = "";
        private string hoverHighlightPath = "";

        private ContentInfo contentInfo;
        private GUIScrollArea contentScrollArea;
        private GUIPanel scrollAreaPanel;
        private GUILayoutX searchBarLayout;
        private GUIButton optionsButton;
        private GUILayout folderBarLayout;
        private GUILayout folderListLayout;
        private GUITextField searchField;
        private GUITexture dragSelection;

        private ContextMenu entryContextMenu;
        private LibraryDropTarget dropTarget;

        private List<ElementEntry> entries = new List<ElementEntry>();
        private Dictionary<string, ElementEntry> entryLookup = new Dictionary<string, ElementEntry>();

        private int autoScrollAmount;
        private bool isDraggingSelection;
        private Vector2I dragSelectionStart;
        private Vector2I dragSelectionEnd;

        private ElementEntry inProgressRenameElement;

        // Cut/Copy/Paste
        private List<string> copyPaths = new List<string>();
        private List<string> cutPaths = new List<string>();

        internal ProjectViewType ViewType
        {
            get { return viewType; }
            set { viewType = value; Refresh(); }
        }

        [MenuItem("Windows/Library", ButtonModifier.CtrlAlt, ButtonCode.L)]
        private static void OpenLibraryWindow()
        {
            OpenWindow<LibraryWindow>();
        }

        protected override LocString GetDisplayName()
        {
            return new LocEdString("Library");
        }

        private void OnInitialize()
        {
            ProjectLibrary.OnEntryAdded += OnEntryChanged;
            ProjectLibrary.OnEntryRemoved += OnEntryChanged;

            GUILayoutY contentLayout = GUI.AddLayoutY();

            searchBarLayout = contentLayout.AddLayoutX();
            searchField = new GUITextField();
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

            folderBarLayout = contentLayout.AddLayoutX();
            GUIButton homeButton = new GUIButton("H", GUIOption.FixedWidth(FOLDER_BUTTON_WIDTH));
            homeButton.OnClick += OnHomeClicked;
            GUIButton upButton = new GUIButton("U", GUIOption.FixedWidth(FOLDER_BUTTON_WIDTH));
            upButton.OnClick += OnUpClicked;

            folderBarLayout.AddElement(homeButton);
            folderBarLayout.AddElement(upButton);
            folderBarLayout.AddSpace(10);

            contentScrollArea = new GUIScrollArea(GUIOption.FlexibleWidth(), GUIOption.FlexibleHeight());
            contentLayout.AddElement(contentScrollArea);
            contentLayout.AddFlexibleSpace();

            entryContextMenu = new ContextMenu();
            entryContextMenu.AddItem("Rename", RenameSelection, new ShortcutKey(ButtonModifier.None, ButtonCode.F2));
            entryContextMenu.AddSeparator("");
            entryContextMenu.AddItem("Cut", CutSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.X));
            entryContextMenu.AddItem("Copy", CopySelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.C));
            entryContextMenu.AddItem("Duplicate", DuplicateSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.D));
            entryContextMenu.AddItem("Paste", PasteToSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.V));
            entryContextMenu.AddSeparator("");
            entryContextMenu.AddItem("Delete", DeleteSelection, new ShortcutKey(ButtonModifier.None, ButtonCode.Delete));

            entryContextMenu.SetLocalizedName("Rename", new LocEdString("Rename"));
            entryContextMenu.SetLocalizedName("Cut", new LocEdString("Cut"));
            entryContextMenu.SetLocalizedName("Copy", new LocEdString("Copy"));
            entryContextMenu.SetLocalizedName("Duplicate", new LocEdString("Duplicate"));
            entryContextMenu.SetLocalizedName("Paste", new LocEdString("Paste"));
            entryContextMenu.SetLocalizedName("Delete", new LocEdString("Delete"));

            Reset();

            dropTarget = new LibraryDropTarget(this);
            dropTarget.Bounds = contentScrollArea.Bounds;
            dropTarget.OnStart += DoOnDragStart;
            dropTarget.OnDrag += DoOnDragMove;
            dropTarget.OnLeave += DoOnDragLeave;
            dropTarget.OnDropResource += DoOnResourceDragDropped;
            dropTarget.OnDropSceneObject += DoOnSceneObjectDragDropped;
            dropTarget.OnEnd += DoOnDragEnd;

            Selection.OnSelectionChanged += OnSelectionChanged;
            Selection.OnResourcePing += OnPing;
        }

        private void OnDestroy()
        {
            Selection.OnSelectionChanged -= OnSelectionChanged;
            Selection.OnResourcePing -= OnPing;
        }

        private ElementEntry FindElementAt(Vector2I windowPos)
        {
            Vector2I scrollPos = WindowToScrollAreaCoords(windowPos);

            foreach (var element in entries)
            {
                if (element.bounds.Contains(scrollPos))
                    return element;
            }

            return null;
        }

        private ElementEntry[] FindElementsOverlapping(Rect2I scrollBounds)
        {
            List<ElementEntry> elements = new List<ElementEntry>();
            foreach (var element in entries)
            {
                if(element.Bounds.Overlaps(scrollBounds))
                    elements.Add(element);
            }

            return elements.ToArray();
        }

        private void DoOnDragStart(Vector2I windowPos)
        {
            Debug.Log("DRAG STARTED + " + windowPos + " - " + GUI.Bounds);

            ElementEntry underCursorElem = FindElementAt(windowPos);
            if (underCursorElem == null)
            {
                StartDragSelection(windowPos);
                return;
            }

            string resourceDir = ProjectLibrary.ResourceFolder;

            string[] dragPaths = null;
            if (selectionPaths.Count > 0)
            {
                foreach(var path in selectionPaths)
                {
                    if (path == underCursorElem.path)
                    {
                        dragPaths = new string[selectionPaths.Count];
                        for (int i = 0; i < selectionPaths.Count; i++)
                        {
                            dragPaths[i] = Path.Combine(resourceDir, selectionPaths[i]);
                        }

                        break;
                    }
                }
            }

            if (dragPaths == null)
                dragPaths = new[] { Path.Combine(resourceDir, underCursorElem.path) };

            ResourceDragDropData dragDropData = new ResourceDragDropData(dragPaths);
            DragDrop.StartDrag(dragDropData);
        }

        private void DoOnDragMove(Vector2I windowPos)
        {
            // Auto-scroll
            Rect2I scrollAreaBounds = contentScrollArea.Bounds;
            int scrollAreaTop = scrollAreaBounds.y;
            int scrollAreaBottom = scrollAreaBounds.y + scrollAreaBounds.height;

            if (windowPos.y > scrollAreaTop && windowPos.y <= (scrollAreaTop + DRAG_SCROLL_HEIGHT))
                autoScrollAmount = -DRAG_SCROLL_AMOUNT_PER_SECOND;
            else if (windowPos.y >= (scrollAreaBottom - DRAG_SCROLL_HEIGHT) && windowPos.y < scrollAreaBottom)
                autoScrollAmount = DRAG_SCROLL_AMOUNT_PER_SECOND;
            else
                autoScrollAmount = 0;

            // Selection box
            if (UpdateDragSelection(windowPos))
                return;

            // Drag and drop (hover element under cursor)
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
        }

        private void DoOnDragLeave()
        {
            ClearHoverHighlight();
            autoScrollAmount = 0;
        }

        private void DoOnResourceDragDropped(Vector2I windowPos, string[] paths)
        {
            ClearHoverHighlight();
            autoScrollAmount = 0;

            if (EndDragSelection())
                return;

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

                    string pathTail = PathEx.GetTail(absolutePath);
                    string destination = Path.Combine(destinationFolder, pathTail);

                    bool doCopy = !ProjectLibrary.Exists(path);

                    if (Directory.Exists(path))
                    {
                        if (doCopy)
                            DirectoryEx.Copy(path, GetUniquePath(destination));
                        else
                            DirectoryEx.Move(path, GetUniquePath(destination));
                    }
                    else if (File.Exists(path))
                    {
                        if (doCopy)
                            FileEx.Copy(path, GetUniquePath(destination));
                        else
                            FileEx.Move(path, GetUniquePath(destination));
                    }

                    ProjectLibrary.Refresh();
                }
            }
        }

        private void DoOnSceneObjectDragDropped(Vector2I windowPos, SceneObject[] objects)
        {
            ClearHoverHighlight();
            autoScrollAmount = 0;

            if (EndDragSelection())
                return;

            string destinationFolder = currentDirectory;

            ElementEntry underCursorElement = FindElementAt(windowPos);
            if (underCursorElement != null)
            {
                LibraryEntry entry = ProjectLibrary.GetEntry(underCursorElement.path);
                if (entry != null && entry.Type == LibraryEntryType.Directory)
                    destinationFolder = entry.Path;
            }

            if (objects != null)
            {
                foreach (var so in objects)
                {
                    if (so == null)
                        continue;

                    Prefab newPrefab = new Prefab(so);

                    string destination = GetUniquePath(Path.Combine(destinationFolder, so.Name + ".prefab"));
                    ProjectLibrary.Create(newPrefab, destination);

                    ProjectLibrary.Refresh();
                }
            }
        }

        private void DoOnDragEnd(Vector2I windowPos)
        {
            EndDragSelection();
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

        public void Ping(string path)
        {
            if (!string.IsNullOrEmpty(pingPath))
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(pingPath, out entry))
                    entry.MarkAsPinged(false);
            }

            pingPath = path;
            if (!string.IsNullOrEmpty(pingPath))
            {
                ElementEntry entry;
                if (entryLookup.TryGetValue(pingPath, out entry))
                    entry.MarkAsPinged(true);

                ScrollToEntry(pingPath);
            }
        }

        public void Reset()
        {
            currentDirectory = ProjectLibrary.Root.Path;
            selectionAnchorStart = -1;
            selectionAnchorEnd = -1;
            selectionPaths.Clear();
            pingPath = "";
            hoverHighlightPath = "";

            Refresh();
        }

        private void DeselectAll(bool onlyInternal = false)
        {
            SetSelection(new List<string>(), onlyInternal);
            selectionAnchorStart = -1;
            selectionAnchorEnd = -1;
        }

        private void Select(string path)
        {
            ElementEntry entry;
            if (!entryLookup.TryGetValue(path, out entry))
                return;

            bool ctrlDown = Input.IsButtonHeld(ButtonCode.LeftControl) || Input.IsButtonHeld(ButtonCode.RightControl);
            bool shiftDown = Input.IsButtonHeld(ButtonCode.LeftShift) || Input.IsButtonHeld(ButtonCode.RightShift);

            if (shiftDown)
            {
                if (selectionAnchorStart != -1 && selectionAnchorStart < entries.Count)
                {
                    int start = Math.Min(entry.index, selectionAnchorStart);
                    int end = Math.Max(entry.index, selectionAnchorStart);

                    List<string> newSelection = new List<string>();
                    for(int i = start; i <= end; i++)
                        newSelection.Add(entries[i].path);

                    SetSelection(newSelection);
                    selectionAnchorEnd = entry.index;
                }
                else
                {
                    SetSelection(new List<string>() {path});
                    selectionAnchorStart = entry.index;
                    selectionAnchorEnd = entry.index;
                }
            }
            else if (ctrlDown)
            {
                List<string> newSelection = new List<string>(selectionPaths);

                if (selectionPaths.Contains(path))
                {
                    newSelection.Remove(path);
                    if (newSelection.Count == 0)
                        DeselectAll();
                    else
                    {
                        if (selectionAnchorStart == entry.index)
                        {
                            ElementEntry newAnchorEntry;
                            if (!entryLookup.TryGetValue(newSelection[0], out newAnchorEntry))
                                selectionAnchorStart = -1;
                            else
                                selectionAnchorStart = newAnchorEntry.index;
                        }

                        if (selectionAnchorEnd == entry.index)
                        {
                            ElementEntry newAnchorEntry;
                            if (!entryLookup.TryGetValue(newSelection[newSelection.Count - 1], out newAnchorEntry))
                                selectionAnchorEnd = -1;
                            else
                                selectionAnchorEnd = newAnchorEntry.index;
                        }

                        SetSelection(newSelection);
                    }
                }
                else
                {
                    newSelection.Add(path);
                    SetSelection(newSelection);
                    selectionAnchorEnd = entry.index;
                }
            }
            else
            {
                SetSelection(new List<string>() {path});

                selectionAnchorStart = entry.index;
                selectionAnchorEnd = entry.index;
            }
        }

        private void MoveSelection(MoveDirection dir)
        {
            string newPath = "";

            if (selectionPaths.Count == 0 || selectionAnchorEnd == -1)
            {
                // Nothing is selected so we arbitrarily select first or last element
                if (entries.Count > 0)
                {
                    switch (dir)
                    {
                        case MoveDirection.Left:
                        case MoveDirection.Up:
                            newPath = entries[entries.Count - 1].path;
                            break;
                        case MoveDirection.Right:
                        case MoveDirection.Down:
                            newPath = entries[0].path;
                            break;
                    }
                }
            }
            else
            {
                switch (dir)
                {
                    case MoveDirection.Left:
                        if (selectionAnchorEnd - 1 >= 0)
                            newPath = entries[selectionAnchorEnd - 1].path;
                        break;
                    case MoveDirection.Up:
                        if (selectionAnchorEnd - contentInfo.elementsPerRow >= 0)
                            newPath = entries[selectionAnchorEnd - contentInfo.elementsPerRow].path;
                        break;
                    case MoveDirection.Right:
                        if (selectionAnchorEnd + 1 < entries.Count)
                            newPath = entries[selectionAnchorEnd + 1].path;
                        break;
                    case MoveDirection.Down:
                        if (selectionAnchorEnd + contentInfo.elementsPerRow < entries.Count)
                            newPath = entries[selectionAnchorEnd + contentInfo.elementsPerRow].path;
                        break;
                }
            }

            if (!string.IsNullOrEmpty(newPath))
            {
                Select(newPath);
                ScrollToEntry(newPath);
            }
        }

        private void SetSelection(List<string> paths, bool onlyInternal = false)
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

            Ping("");
            StopRename();

            if (!onlyInternal)
            {
                if (selectionPaths != null)
                    Selection.resourcePaths = selectionPaths.ToArray();
                else
                    Selection.resourcePaths = new string[0];
            }
        }

        private void EnterDirectory(string directory)
        {
            currentDirectory = directory;
            DeselectAll();

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
                if (Directory.Exists(source))
                    DirectoryEx.Copy(source, GetUniquePath(source));
                else if (File.Exists(source))
                    FileEx.Copy(source, GetUniquePath(source));

                ProjectLibrary.Refresh();
            }
        }

        private void Paste(string destinationFolder)
        {
            if (copyPaths.Count > 0)
            {
                for (int i = 0; i < copyPaths.Count; i++)
                {
                    string destination = Path.Combine(destinationFolder, PathEx.GetTail(copyPaths[i]));

                    if (Directory.Exists(copyPaths[i]))
                        DirectoryEx.Copy(copyPaths[i], GetUniquePath(destination));
                    else if (File.Exists(copyPaths[i]))
                        FileEx.Copy(copyPaths[i], GetUniquePath(destination));
                }

                ProjectLibrary.Refresh();
            }
            else if (cutPaths.Count > 0)
            {
                for (int i = 0; i < cutPaths.Count; i++)
                {
                    string destination = Path.Combine(destinationFolder, PathEx.GetTail(cutPaths[i]));

                    if (Directory.Exists(cutPaths[i]))
                        DirectoryEx.Move(cutPaths[i], GetUniquePath(destination));
                    else if (File.Exists(cutPaths[i]))
                        FileEx.Move(cutPaths[i], GetUniquePath(destination));
                }

                cutPaths.Clear();
                ProjectLibrary.Refresh();
            }
        }

        private string GetUniquePath(string path)
        {
            string extension = Path.GetExtension(path);
            string pathNoExtension = path;
            if (!string.IsNullOrEmpty(extension))
                pathNoExtension = path.Remove(path.Length - extension.Length);

            int idx = 0;
            string destination = path;
            while (ProjectLibrary.Exists(destination))
            {
                destination = pathNoExtension + "_" + idx;
                idx++;
            }

            return destination + extension;
        }

        private void OnEditorUpdate()
        {
            bool isRenameInProgress = inProgressRenameElement != null;

            if (HasContentFocus)
            {
                if (!isRenameInProgress)
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

                    if (Input.IsButtonDown(ButtonCode.Return))
                    {
                        if (selectionPaths.Count == 1)
                        {
                            LibraryEntry entry = ProjectLibrary.GetEntry(selectionPaths[0]);
                            if (entry != null && entry.Type == LibraryEntryType.Directory)
                            {
                                EnterDirectory(entry.Path);
                            }
                        }
                    }
                    else if (Input.IsButtonDown(ButtonCode.Back))
                    {
                        LibraryEntry entry = ProjectLibrary.GetEntry(currentDirectory);
                        if (entry != null && entry.Parent != null)
                        {
                            EnterDirectory(entry.Parent.Path);
                        }
                    }
                    else if (Input.IsButtonDown(ButtonCode.Up))
                    {
                        MoveSelection(MoveDirection.Up);
                    }
                    else if (Input.IsButtonDown(ButtonCode.Down))
                    {
                        MoveSelection(MoveDirection.Down);
                    }
                    else if (Input.IsButtonDown(ButtonCode.Left))
                    {
                        MoveSelection(MoveDirection.Left);
                    }
                    else if (Input.IsButtonDown(ButtonCode.Right))
                    {
                        MoveSelection(MoveDirection.Right);
                    }
                    else if (Input.IsButtonDown(ButtonCode.F2))
                    {
                        RenameSelection();
                    }
                    else if (Input.IsButtonDown(ButtonCode.Delete))
                    {
                        DeleteSelection();
                    }
                }
                else
                {
                    if (Input.IsButtonDown(ButtonCode.Return))
                    {
                        string newName = inProgressRenameElement.GetRenamedName();

                        string originalPath = inProgressRenameElement.path;
                        originalPath = originalPath.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);

                        string newPath = Path.GetDirectoryName(originalPath);
                        newPath = Path.Combine(newPath, newName + Path.GetExtension(originalPath));

                        bool renameOK = true;
                        if (!PathEx.IsValidFileName(newName))
                        {
                            DialogBox.Open(new LocEdString("Error"), new LocEdString("The name you specified is not a valid file name. Try another."), DialogBox.Type.OK);
                            renameOK = false;
                        }

                        if (renameOK)
                        {
                            // Windows sees paths with dot at the end as if they didn't have it
                            // so remove the dot to ensure the project library does the same
                            string trimmedNewPath = newPath.TrimEnd('.');

                            if (originalPath != trimmedNewPath && ProjectLibrary.Exists(trimmedNewPath))
                            {
                                DialogBox.Open(new LocEdString("Error"), new LocEdString("File/folder with that name already exists in this folder."), DialogBox.Type.OK);
                                renameOK = false;
                            }
                        }

                        if (renameOK)
                        {
                            ProjectLibrary.Rename(originalPath, newPath);
                            StopRename();
                        }
                    }
                    else if (Input.IsButtonDown(ButtonCode.Escape))
                    {
                        StopRename();
                    }
                }
            }
            else
            {
                if(isRenameInProgress)
                    StopRename();
            }

            if (autoScrollAmount != 0)
            {
                Rect2I contentBounds = contentScrollArea.ContentBounds;
                float scrollPct = autoScrollAmount / (float)contentBounds.height;

                contentScrollArea.VerticalScroll += scrollPct * Time.FrameDelta;
            }

            if (requiresRefresh)
                Refresh();

            dropTarget.Update();
        }

        private void OnEntryChanged(string entry)
        {
            requiresRefresh = true;
        }

        private void ScrollToEntry(string path)
        {
            ElementEntry entryGUI;
            if (!entryLookup.TryGetValue(path, out entryGUI))
                return;

            Rect2I entryBounds = entryGUI.Bounds;

            Rect2I contentBounds = contentScrollArea.Layout.Bounds;
            Rect2I windowEntryBounds = entryBounds;
            windowEntryBounds.x += contentBounds.x;
            windowEntryBounds.y += contentBounds.y;

            Rect2I scrollAreaBounds = contentScrollArea.Bounds;
            bool requiresScroll = windowEntryBounds.y < scrollAreaBounds.y ||
                                  (windowEntryBounds.y + windowEntryBounds.height) > (scrollAreaBounds.y + scrollAreaBounds.height);

            if (!requiresScroll)
                return;

            int scrollableSize = contentBounds.height - scrollAreaBounds.height;
            float percent = (((entryBounds.y + entryBounds.height * 0.5f) - scrollAreaBounds.height * 0.5f) / (float)scrollableSize);

            percent = MathEx.Clamp01(percent);
            contentScrollArea.VerticalScroll = percent;
        }

        private void Refresh()
        {
            requiresRefresh = false;

            LibraryEntry[] entriesToDisplay = new LibraryEntry[0];
            if (IsSearchActive)
            {
                entriesToDisplay = ProjectLibrary.Search("*" + searchQuery + "*");
            }
            else
            {
                DirectoryEntry entry = ProjectLibrary.GetEntry(currentDirectory) as DirectoryEntry;
                if (entry == null)
                {
                    currentDirectory = ProjectLibrary.Root.Path;
                    entry = ProjectLibrary.GetEntry(currentDirectory) as DirectoryEntry;
                }

                if(entry != null)
                    entriesToDisplay = entry.Children;
            }

            if (scrollAreaPanel != null)
                scrollAreaPanel.Destroy();

            entries.Clear();
            entryLookup.Clear();
            inProgressRenameElement = null;

            scrollAreaPanel = contentScrollArea.Layout.AddPanel();

            RefreshDirectoryBar();
            SortEntries(entriesToDisplay);

            if (entriesToDisplay.Length == 0)
                return;

            contentInfo = new ContentInfo(this, viewType, entriesToDisplay.Length);

            if (viewType == ProjectViewType.List16)
            {
                for (int i = 0; i < entriesToDisplay.Length; i++)
                {
                    ElementEntry guiEntry = new ElementEntry(contentInfo, contentInfo.main, entriesToDisplay[i], i);
                    entries.Add(guiEntry);
                    entryLookup[guiEntry.path] = guiEntry;

                    if (i != entriesToDisplay.Length - 1)
                        contentInfo.main.AddSpace(LIST_ENTRY_SPACING);
                }

                contentInfo.main.AddFlexibleSpace();
            }
            else
            {
                contentInfo.main.AddSpace(GRID_ENTRY_SPACING / 2);
                GUILayoutX rowLayout = contentInfo.main.AddLayoutX();
                contentInfo.main.AddSpace(GRID_ENTRY_SPACING);
                rowLayout.AddFlexibleSpace();

                int elemsInRow = 0;

                for (int i = 0; i < entriesToDisplay.Length; i++)
                {
                    if (elemsInRow == contentInfo.elementsPerRow && elemsInRow > 0)
                    {
                        rowLayout = contentInfo.main.AddLayoutX();
                        contentInfo.main.AddSpace(GRID_ENTRY_SPACING);

                        rowLayout.AddFlexibleSpace();
                        elemsInRow = 0;
                    }

                    ElementEntry guiEntry = new ElementEntry(contentInfo, rowLayout, entriesToDisplay[i], i);
                    entries.Add(guiEntry);
                    entryLookup[guiEntry.path] = guiEntry;

                    rowLayout.AddFlexibleSpace();

                    elemsInRow++;
                }

                int extraElements = contentInfo.elementsPerRow - elemsInRow;
                for (int i = 0; i < extraElements; i++)
                {
                    rowLayout.AddSpace(contentInfo.labelWidth);
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
            contentBounds.height = Math.Max(contentBounds.height, minimalBounds.height);

            GUIButton catchAll = new GUIButton("", EditorStyles.Blank);
            catchAll.Bounds = contentBounds;
            catchAll.OnClick += OnCatchAllClicked;
            catchAll.SetContextMenu(entryContextMenu);

            contentInfo.underlay.AddElement(catchAll);

            Rect2I focusBounds = contentBounds; // Contents + Folder bar
            Rect2I scrollBounds = contentScrollArea.Bounds;
            focusBounds.x += scrollBounds.x;
            focusBounds.y += scrollBounds.y;

            Rect2I folderBarBounds = folderListLayout.Bounds;
            focusBounds.y -= folderBarBounds.height;
            focusBounds.height += folderBarBounds.height;

            GUIButton focusCatcher = new GUIButton("", EditorStyles.Blank);
            focusCatcher.OnFocusChanged += OnContentsFocusChanged;
            focusCatcher.Bounds = focusBounds;

            GUIPanel focusPanel = GUI.AddPanel(3);
            focusPanel.AddElement(focusCatcher);

            UpdateDragSelection(dragSelectionEnd);
        }

        private Vector2I WindowToScrollAreaCoords(Vector2I windowPos)
        {
            Rect2I scrollBounds = contentScrollArea.Layout.Bounds;
            Vector2I scrollPos = windowPos;
            scrollPos.x -= scrollBounds.x;
            scrollPos.y -= scrollBounds.y;

            return scrollPos;
        }

        private void StartDragSelection(Vector2I windowPos)
        {
            isDraggingSelection = true;
            dragSelectionStart = WindowToScrollAreaCoords(windowPos);
            dragSelectionEnd = dragSelectionStart;
        }

        private bool UpdateDragSelection(Vector2I windowPos)
        {
            if (!isDraggingSelection)
                return false;

            if (dragSelection == null)
            {
                dragSelection = new GUITexture(null, true, EditorStyles.SelectionArea);
                contentInfo.overlay.AddElement(dragSelection);
            }

            dragSelectionEnd = WindowToScrollAreaCoords(windowPos);

            Rect2I selectionArea = CalculateSelectionArea();
            SelectInArea(selectionArea);
            dragSelection.Bounds = selectionArea;

            return true;
        }

        private bool EndDragSelection()
        {
            if (!isDraggingSelection)
                return false;

            if (dragSelection != null)
            {
                dragSelection.Destroy();
                dragSelection = null;
            }

            Rect2I selectionArea = CalculateSelectionArea();
            SelectInArea(selectionArea);

            isDraggingSelection = false;
            return false;
        }

        private Rect2I CalculateSelectionArea()
        {
            Rect2I selectionArea = new Rect2I();
            if (dragSelectionStart.x < dragSelectionEnd.x)
            {
                selectionArea.x = dragSelectionStart.x;
                selectionArea.width = dragSelectionEnd.x - dragSelectionStart.x;
            }
            else
            {
                selectionArea.x = dragSelectionEnd.x;
                selectionArea.width = dragSelectionStart.x - dragSelectionEnd.x;
            }

            if (dragSelectionStart.y < dragSelectionEnd.y)
            {
                selectionArea.y = dragSelectionStart.y;
                selectionArea.height = dragSelectionEnd.y - dragSelectionStart.y;
            }
            else
            {
                selectionArea.y = dragSelectionEnd.y;
                selectionArea.height = dragSelectionStart.y - dragSelectionEnd.y;
            }

            Rect2I maxBounds = contentScrollArea.Layout.Bounds;
            maxBounds.x = 0;
            maxBounds.y = 0;

            selectionArea.Clip(maxBounds);

            return selectionArea;
        }

        private void SelectInArea(Rect2I scrollBounds)
        {
            ElementEntry[] foundElements = FindElementsOverlapping(scrollBounds);

            if (foundElements.Length > 0)
            {
                selectionAnchorStart = foundElements[0].index;
                selectionAnchorEnd = foundElements[foundElements.Length - 1].index;
            }
            else
            {
                selectionAnchorStart = -1;
                selectionAnchorEnd = -1;
            }

            List<string> elementPaths = new List<string>();
            foreach (var elem in foundElements)
                elementPaths.Add(elem.path);

            SetSelection(elementPaths);
        }

        private void RefreshDirectoryBar()
        {
            if (folderListLayout != null)
            {
                folderListLayout.Destroy();
                folderListLayout = null;
            }

            folderListLayout = folderBarLayout.AddLayoutX();

            string[] folders = null;
            string[] fullPaths = null;

            if (IsSearchActive)
            {
                folders = new[] {searchQuery};
                fullPaths = new[] { searchQuery };
            }
            else
            {
                string currentDir = Path.Combine("Resources", currentDirectory);

                folders = currentDir.Split(new[] { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar },
                    StringSplitOptions.RemoveEmptyEntries);

                fullPaths = new string[folders.Length];
                for (int i = 0; i < folders.Length; i++)
                {
                    if (i == 0)
                        fullPaths[i] = "";
                    else
                        fullPaths[i] = Path.Combine(fullPaths[i - 1], folders[i]);
                }
            }

            int availableWidth = folderBarLayout.Bounds.width - FOLDER_BUTTON_WIDTH * 2;
            int numFolders = 0;
            for (int i = folders.Length - 1; i >= 0; i--)
            {
                GUIButton folderButton = new GUIButton(folders[i]);

                if (!IsSearchActive)
                {
                    string fullPath = fullPaths[i];
                    folderButton.OnClick += () => OnFolderButtonClicked(fullPath);
                }

                GUILabel separator = new GUILabel("/", GUIOption.FixedWidth(FOLDER_SEPARATOR_WIDTH));

                folderListLayout.InsertElement(0, separator);
                folderListLayout.InsertElement(0, folderButton);
                numFolders++;

                Rect2I folderListBounds = folderListLayout.Bounds;
                if (folderListBounds.width > availableWidth)
                {
                    if (numFolders > 2)
                    {
                        separator.Destroy();
                        folderButton.Destroy();
                        break;
                    }
                }
            }
        }

        private void SortEntries(LibraryEntry[] input)
        {
            Array.Sort(input, (x, y) =>
            {
                if (x.Type == y.Type)
                    return x.Name.CompareTo(y.Name);
                else
                    return x.Type == LibraryEntryType.File ? 1 : -1;
            });
        }

        private void OnSelectionChanged(SceneObject[] sceneObjects, string[] resourcePaths)
        {
            if(sceneObjects.Length > 0)
                DeselectAll(true);
        }

        private void OnPing(string path)
        {
            Ping(path);
        }

        private void OnFolderButtonClicked(string path)
        {
            EnterDirectory(path);
        }

        private void OnContentsFocusChanged(bool focus)
        {
            hasContentFocus = focus;
        }

        private void OnEntryClicked(string path)
        {
            Select(path);
        }

        private void OnEntryDoubleClicked(string path)
        {
            LibraryEntry entry = ProjectLibrary.GetEntry(path);
            if (entry != null)
            {
                if (entry.Type == LibraryEntryType.Directory)
                    EnterDirectory(path);
                else
                {
                    FileEntry resEntry = (FileEntry) entry;
                    if (resEntry.ResType == ResourceType.Prefab)
                    {
                        EditorApplication.LoadScene(resEntry.Path);
                    }
                }
            }
        }

        private void OnCatchAllClicked()
        {
            DeselectAll();
        }

        private void OnHomeClicked()
        {
            currentDirectory = ProjectLibrary.Root.Path;
            Refresh();
        }

        private void OnUpClicked()
        {
            currentDirectory = currentDirectory.Trim(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);

            if (!string.IsNullOrEmpty(currentDirectory))
            {
                string parent = Path.GetDirectoryName(currentDirectory);

                currentDirectory = parent;
                Refresh();
            }
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

        private void RenameSelection()
        {
            if (selectionPaths.Count == 0)
                return;

            if (selectionPaths.Count > 1)
            {
                DeselectAll();
                Select(selectionPaths[0]);
            }

            ElementEntry entry;
            if (entryLookup.TryGetValue(selectionPaths[0], out entry))
            {
                entry.StartRename();
                inProgressRenameElement = entry;
            }
        }

        private void StopRename()
        {
            if (inProgressRenameElement != null)
            {
                inProgressRenameElement.StopRename();
                inProgressRenameElement = null;
            }
        }

        private void DeleteSelection()
        {
            if (selectionPaths.Count == 0)
                return;

            DialogBox.Open(new LocEdString("Confirm deletion"), new LocEdString("Are you sure you want to delete the selected object(s)?"),
                DialogBox.Type.YesNo,
                type =>
                {
                    if (type == DialogBox.ResultType.Yes)
                    {
                        foreach (var path in selectionPaths)
                        {
                            ProjectLibrary.Delete(path);
                        }

                        DeselectAll();
                        Refresh();
                    }
                });
        }

        private void OnSearchChanged(string newValue)
        {
            searchQuery = newValue;
            Refresh();
        }

        private void ClearSearch()
        {
            searchField.Value = "";
            searchQuery = "";
            Refresh();
        }

        private void OpenOptionsWindow()
        {
            Vector2I openPosition;
            Rect2I buttonBounds = GUILayoutUtility.CalculateBounds(optionsButton, GUI);

            openPosition.x = buttonBounds.x + buttonBounds.width / 2;
            openPosition.y = buttonBounds.y + buttonBounds.height / 2;

            LibraryDropDown dropDown = DropDownWindow.Open<LibraryDropDown>(this, openPosition);
            dropDown.SetParent(this);
        }

        private Rect2I GetScrollAreaBounds()
        {
            Rect2I bounds = GUI.Bounds;
            Rect2I folderListBounds = folderListLayout.Bounds;
            Rect2I searchBarBounds = searchBarLayout.Bounds;

            bounds.y += folderListBounds.height + searchBarBounds.height;
            bounds.height -= folderListBounds.height + searchBarBounds.height;

            return bounds;
        }

        protected override void WindowResized(int width, int height)
        {
            base.WindowResized(width, height);

            Refresh();

            dropTarget.Bounds = contentScrollArea.Bounds;
        }

        private class ContentInfo
        {
            public ContentInfo(LibraryWindow window, ProjectViewType viewType, int numEntries)
            {
                GUIPanel parentPanel = window.scrollAreaPanel;

                GUIPanel contentPanel = parentPanel.AddPanel(1);
                overlay = parentPanel.AddPanel(0);
                underlay = parentPanel.AddPanel(2);
                inputOverlay = parentPanel.AddPanel(-1);

                main = contentPanel.AddLayoutY();

                if (viewType == ProjectViewType.List16)
                {
                    tileSize = 16;
                    gridLayout = false;
                    elementsPerRow = 1;
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

                    Rect2I scrollBounds = window.contentScrollArea.Bounds;
                    int availableWidth = scrollBounds.width;

                    int elemSize = tileSize + GRID_ENTRY_SPACING;
                    elementsPerRow = (availableWidth - GRID_ENTRY_SPACING * 2) / elemSize;
                    elementsPerRow = Math.Max(elementsPerRow, 1);

                    int numRows = MathEx.CeilToInt(numEntries / (float)elementsPerRow);
                    int neededHeight = numRows * (elemSize);

                    bool requiresScrollbar = neededHeight > scrollBounds.height;
                    if (requiresScrollbar)
                    {
                        availableWidth -= window.contentScrollArea.ScrollBarWidth;
                        elementsPerRow = (availableWidth - GRID_ENTRY_SPACING * 2) / elemSize;
                    }

                    if (elementsPerRow > 0)
                        labelWidth = (availableWidth - (elementsPerRow + 1)*MIN_HORZ_SPACING)/elementsPerRow;
                    else
                        labelWidth = 0;
                }

                this.window = window;
            }

            public GUILayout main;
            public GUIPanel overlay;
            public GUIPanel underlay;
            public GUIPanel inputOverlay;

            public LibraryWindow window;
            public int tileSize;
            public bool gridLayout;

            public int elementsPerRow;
            public int labelWidth;
        }

        private class ElementEntry
        {
            // Note: Order of these is relevant
            enum UnderlayState
            {
                None, Hovered, Selected, Pinged
            }

            public int index;
            public string path;
            public GUITexture icon;
            public GUILabel label;
            public Rect2I bounds;

            private GUITexture underlay;
            private ContentInfo info;
            private UnderlayState underlayState;
            private GUITextBox renameTextBox;

            public ElementEntry(ContentInfo info, GUILayout parent, LibraryEntry entry, int index)
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
                    label = new GUILabel(entry.Name, EditorStyles.MultiLineLabelCentered,
                        GUIOption.FixedWidth(info.labelWidth), GUIOption.FlexibleHeight(0, MAX_LABEL_HEIGHT));
                }
                else
                {
                    label = new GUILabel(entry.Name);
                }

                entryLayout.AddElement(icon);
                entryLayout.AddElement(label);

                this.info = info;
                this.index = index;
                this.path = entry.Path;
                this.bounds = new Rect2I();
                this.underlay = null;
            }

            public void Initialize()
            {
                bounds = icon.Bounds;
                Rect2I labelBounds = label.Bounds;

                bounds.x = MathEx.Min(bounds.x, labelBounds.x - SELECTION_EXTRA_WIDTH);
                bounds.y = MathEx.Min(bounds.y, labelBounds.y) - 5; // 5 - Just padding for better look
                bounds.width = MathEx.Max(bounds.x + bounds.width,
                    labelBounds.x + labelBounds.width) - bounds.x + SELECTION_EXTRA_WIDTH;
                bounds.height = MathEx.Max(bounds.y + bounds.height,
                    labelBounds.y + labelBounds.height) - bounds.y;

                LibraryWindow hoistedWindow = info.window;
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
                if ((int)underlayState > (int)UnderlayState.Selected)
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

            public void StartRename()
            {
                if (renameTextBox != null)
                    return;

                renameTextBox = new GUITextBox(false);
                renameTextBox.Bounds = label.Bounds;
                info.inputOverlay.AddElement(renameTextBox);

                string name = Path.GetFileNameWithoutExtension(PathEx.GetTail(path));
                renameTextBox.Text = name;
                renameTextBox.Focus = true;

                label.Visible = false;
            }

            public void StopRename()
            {
                if (renameTextBox != null)
                {
                    renameTextBox.Destroy();
                    renameTextBox = null;
                }

                label.Visible = true;
            }

            public string GetRenamedName()
            {
                if (renameTextBox != null)
                    return renameTextBox.Text;

                return "";
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
                            return EditorBuiltin.PlainTextIcon;
                        case ResourceType.ScriptCode:
                            return EditorBuiltin.ScriptCodeIcon;
                        case ResourceType.SpriteTexture:
                            return EditorBuiltin.SpriteTextureIcon;
                        case ResourceType.Shader:
                            return EditorBuiltin.ShaderIcon;
                        case ResourceType.Material:
                            return EditorBuiltin.MaterialIcon;
                        case ResourceType.Prefab:
                            return EditorBuiltin.PrefabIcon;
                    }
                }

                return null;
            }
        }

        enum MoveDirection
        {
            Up, Down, Left, Right
        }
    }

    internal class LibraryDropDown : DropDownWindow
    {
        private LibraryWindow parent;

        public LibraryDropDown()
            :base(150, 30)
        { }

        internal void SetParent(LibraryWindow parent)
        {
            this.parent = parent;

            GUIToggleGroup group = new GUIToggleGroup();

            GUIToggle list16 = new GUIToggle(new LocEdString("16"), group, EditorStyles.Button, GUIOption.FixedWidth(30));
            GUIToggle grid32 = new GUIToggle(new LocEdString("32"), group, EditorStyles.Button, GUIOption.FixedWidth(30));
            GUIToggle grid48 = new GUIToggle(new LocEdString("48"), group, EditorStyles.Button, GUIOption.FixedWidth(30));
            GUIToggle grid64 = new GUIToggle(new LocEdString("64"), group, EditorStyles.Button, GUIOption.FixedWidth(30));

            ProjectViewType activeType = parent.ViewType;
            switch (activeType)
            {
                case ProjectViewType.List16:
                    list16.Value = true;
                    break;
                case ProjectViewType.Grid32:
                    grid32.Value = true;
                    break;
                case ProjectViewType.Grid48:
                    grid48.Value = true;
                    break;
                case ProjectViewType.Grid64:
                    grid64.Value = true;
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
