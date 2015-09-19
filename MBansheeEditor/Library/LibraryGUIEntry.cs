using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal class LibraryGUIEntry
    {
        private const int MAX_LABEL_HEIGHT = 50;

        private static readonly Color PING_COLOR = Color.BansheeOrange;
        private static readonly Color SELECTION_COLOR = Color.DarkCyan;
        private static readonly Color HOVER_COLOR = new Color(Color.DarkCyan.r, Color.DarkCyan.g, Color.DarkCyan.b, 0.5f);
        private static readonly Color CUT_COLOR = new Color(1.0f, 1.0f, 1.0f, 0.5f);
        private const int SELECTION_EXTRA_WIDTH = 3;

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
        private LibraryGUIContent owner;
        private UnderlayState underlayState;
        private GUITextBox renameTextBox;

        public LibraryGUIEntry(LibraryGUIContent owner, GUILayout parent, LibraryEntry entry, int index)
        {
            GUILayout entryLayout;

            if (owner.GridLayout)
                entryLayout = parent.AddLayoutY();
            else
                entryLayout = parent.AddLayoutX();

            SpriteTexture iconTexture = GetIcon(entry);

            icon = new GUITexture(iconTexture, GUIImageScaleMode.ScaleToFit,
                true, GUIOption.FixedHeight(owner.TileSize), GUIOption.FixedWidth(owner.TileSize));

            label = null;

            if (owner.GridLayout)
            {
                label = new GUILabel(entry.Name, EditorStyles.MultiLineLabelCentered,
                    GUIOption.FixedWidth(owner.LabelWidth), GUIOption.FlexibleHeight(0, MAX_LABEL_HEIGHT));
            }
            else
            {
                label = new GUILabel(entry.Name);
            }

            entryLayout.AddElement(icon);
            entryLayout.AddElement(label);

            this.owner = owner;
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

            string hoistedPath = path;

            GUIButton overlayBtn = new GUIButton("", EditorStyles.Blank);
            overlayBtn.Bounds = bounds;
            overlayBtn.OnClick += () => OnEntryClicked(hoistedPath);
            overlayBtn.OnDoubleClick += () => OnEntryDoubleClicked(hoistedPath);
            overlayBtn.SetContextMenu(owner.Window.ContextMenu);

            owner.Overlay.AddElement(overlayBtn);
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
            owner.InputOverlay.AddElement(renameTextBox);

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

                owner.Underlay.AddElement(underlay);
            }
        }

        private void OnEntryClicked(string path)
        {
            owner.Window.Select(path);
        }

        private void OnEntryDoubleClicked(string path)
        {
            LibraryEntry entry = ProjectLibrary.GetEntry(path);
            if (entry != null)
            {
                if (entry.Type == LibraryEntryType.Directory)
                    owner.Window.EnterDirectory(path);
                else
                {
                    FileEntry resEntry = (FileEntry)entry;
                    if (resEntry.ResType == ResourceType.Prefab)
                    {
                        EditorApplication.LoadScene(resEntry.Path);
                    }
                }
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
}
