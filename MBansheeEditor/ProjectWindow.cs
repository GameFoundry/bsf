using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class ProjectWindow : EditorWindow
    {
        private enum ViewType
        {
            Grid64, Grid48, Grid32, List16
        }

        private const int GRID_ENTRY_SPACING = 15;
        private const int LIST_ENTRY_SPACING = 7;
        private const int MAX_LABEL_HEIGHT = 50;

        private bool hasContentFocus = false;
        private bool HasContentFocus { get { return HasFocus && hasContentFocus; } }

        private ViewType viewType = ViewType.Grid32;

        private string currentDirectory;
        private string currentSelection;
        private string currentPing;

        private GUIScrollArea contentScrollArea;
        private GUILayout contentLayout;

        [MenuItem("Windows/Project", ButtonModifier.Ctrl, ButtonCode.P)]
        private static void OpenProjectWindow()
        {
            OpenWindow<ProjectWindow>();
        }

        private void OnInitialize()
        {
            ProjectLibrary.OnEntryAdded += OnEntryChanged;
            ProjectLibrary.OnEntryRemoved += OnEntryChanged;

            contentScrollArea = new GUIScrollArea(GUIOption.FlexibleWidth(), GUIOption.FlexibleHeight());
            GUI.layout.AddElement(contentScrollArea);

            Reset();
        }

        public void Ping(Resource resource)
        {
            currentPing = ProjectLibrary.GetPath(resource);

            Refresh();
        }

        private void Select(Resource resource)
        {
            currentSelection = ProjectLibrary.GetPath(resource);
            currentPing = "";

            Refresh();
        }

        private void EnterDirectory(string directory)
        {
            currentDirectory = directory;
            currentPing = "";
            currentSelection = "";

            Refresh();
        }

        private void SetView(ViewType type)
        {
            viewType = type;
            Refresh();
        }

        private void EditorUpdate()
        {
            // TODO - Handle input, drag and drop and whatever else might be needed
        }

        private void OnEntryChanged(string entry)
        {
            Refresh();
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

            if (contentLayout != null)
                contentLayout.Destroy();

            contentLayout = contentScrollArea.layout.AddLayoutY();

            Rect2I contentBounds = contentScrollArea.Bounds;
            LibraryEntry[] childEntries = entry.Children;

            if (childEntries.Length == 0)
                return;

            if (viewType == ViewType.List16)
            {
                int tileSize = 16;

                for (int i = 0; i < childEntries.Length; i++)
                {
                    // TODO
                }
            }
            else
            {
                int tileSize = 64;
                switch (viewType)
                {
                    case ViewType.Grid64: tileSize = 64; break;
                    case ViewType.Grid48: tileSize = 48; break;
                    case ViewType.Grid32: tileSize = 32; break;
                }

                GUILayoutX rowLayout = contentLayout.AddLayoutX();
                contentLayout.AddFlexibleSpace();

                rowLayout.AddFlexibleSpace();
                int currentWidth = GRID_ENTRY_SPACING * 2;
                bool addedAny = false;

                for (int i = 0; i < childEntries.Length; i++)
                {
                    if (currentWidth >= contentBounds.width && addedAny) // We force at least one entry per row, even if it doesn't fit
                    {
                        rowLayout = contentLayout.AddLayoutX();
                        contentLayout.AddFlexibleSpace();

                        rowLayout.AddFlexibleSpace();

                        currentWidth = GRID_ENTRY_SPACING * 2;
                    }

                    LibraryEntry currentEntry = childEntries[i];

                    GUILayoutY entryLayout = rowLayout.AddLayoutY();
                    rowLayout.AddFlexibleSpace();

                    SpriteTexture iconTexture = GetIcon(currentEntry);

                    GUITexture icon = new GUITexture(iconTexture, GUIImageScaleMode.ScaleToFit, 
                        true, GUIOption.FixedHeight(tileSize), GUIOption.FixedWidth(tileSize));

                    GUILabel label = new GUILabel(currentEntry.Name, EditorStyles.MultiLineLabel, 
                        GUIOption.FixedWidth(tileSize), GUIOption.FlexibleHeight(0, MAX_LABEL_HEIGHT));

                    entryLayout.AddElement(icon);
                    entryLayout.AddElement(label);

                    addedAny = true;
                    currentWidth += tileSize + GRID_ENTRY_SPACING;
                }
            }

            // TODO - Handle ping/selection and whatever else
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
}
