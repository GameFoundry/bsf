using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal class LibraryGUIContent
    {
        private const int MIN_HORZ_SPACING = 8;
        private const int GRID_ENTRY_SPACING = 15;
        private const int LIST_ENTRY_SPACING = 7;

        private GUIPanel mainPanel;
        private GUILayout main;
        private GUIPanel overlay;
        private GUIPanel underlay;
        private GUIPanel inputOverlay;

        private LibraryWindow window;
        private GUIScrollArea parent;
        private int tileSize;
        private bool gridLayout;

        private int elementsPerRow;
        private int labelWidth;

        private LibraryGUIEntry[] entries = new LibraryGUIEntry[0];
        private Dictionary<string, LibraryGUIEntry> entryLookup = new Dictionary<string, LibraryGUIEntry>();

        public Rect2I Bounds
        {
            get { return main.Bounds; }
        }

        public int ElementsPerRow
        {
            get { return elementsPerRow; }
        }

        public int LabelWidth
        {
            get { return labelWidth; }
        }

        public bool GridLayout
        {
            get { return gridLayout; }
        }

        public int TileSize
        {
            get { return tileSize; }
        }

        public LibraryGUIEntry[] Entries
        {
            get { return entries; }
        }

        // TODO: This doesn't feel like it should be public
        public LibraryWindow Window
        {
            get { return window; }
        }

        // TODO: This doesn't feel like it should be public
        public GUIPanel Underlay
        {
            get { return underlay; }
        }

        // TODO: This doesn't feel like it should be public
        public GUIPanel Overlay
        {
            get { return overlay; }
        }

        // TODO: This doesn't feel like it should be public
        public GUIPanel InputOverlay
        {
            get { return inputOverlay; }
        }

        public LibraryGUIContent(LibraryWindow window, GUIScrollArea parent)
        {
            this.window = window;
            this.parent = parent;
        }

        public void Refresh(ProjectViewType viewType, LibraryEntry[] entriesToDisplay)
        {
            if (mainPanel != null)
                mainPanel.Destroy();

            entries = new LibraryGUIEntry[entriesToDisplay.Length];
            entryLookup.Clear();

            mainPanel = parent.Layout.AddPanel();

            GUIPanel contentPanel = mainPanel.AddPanel(1);
            overlay = mainPanel.AddPanel(0);
            underlay = mainPanel.AddPanel(2);
            inputOverlay = mainPanel.AddPanel(-1);

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

                Rect2I scrollBounds = parent.Bounds;
                int availableWidth = scrollBounds.width;

                int elemSize = tileSize + GRID_ENTRY_SPACING;
                elementsPerRow = (availableWidth - GRID_ENTRY_SPACING * 2) / elemSize;
                elementsPerRow = Math.Max(elementsPerRow, 1);

                int numRows = MathEx.CeilToInt(entriesToDisplay.Length / (float)elementsPerRow);
                int neededHeight = numRows * (elemSize);

                bool requiresScrollbar = neededHeight > scrollBounds.height;
                if (requiresScrollbar)
                {
                    availableWidth -= parent.ScrollBarWidth;
                    elementsPerRow = (availableWidth - GRID_ENTRY_SPACING * 2) / elemSize;
                }

                if (elementsPerRow > 0)
                    labelWidth = (availableWidth - (elementsPerRow + 1) * MIN_HORZ_SPACING) / elementsPerRow;
                else
                    labelWidth = 0;
            }

            if (viewType == ProjectViewType.List16)
            {
                for (int i = 0; i < entriesToDisplay.Length; i++)
                {
                    LibraryGUIEntry guiEntry = new LibraryGUIEntry(this, main, entriesToDisplay[i], i);
                    entries[i] = guiEntry;
                    entryLookup[guiEntry.path] = guiEntry;

                    if (i != entriesToDisplay.Length - 1)
                        main.AddSpace(LIST_ENTRY_SPACING);
                }

                main.AddFlexibleSpace();
            }
            else
            {
                main.AddSpace(GRID_ENTRY_SPACING / 2);
                GUILayoutX rowLayout = main.AddLayoutX();
                main.AddSpace(GRID_ENTRY_SPACING);
                rowLayout.AddFlexibleSpace();

                int elemsInRow = 0;

                for (int i = 0; i < entriesToDisplay.Length; i++)
                {
                    if (elemsInRow == elementsPerRow && elemsInRow > 0)
                    {
                        rowLayout = main.AddLayoutX();
                        main.AddSpace(GRID_ENTRY_SPACING);

                        rowLayout.AddFlexibleSpace();
                        elemsInRow = 0;
                    }

                    LibraryGUIEntry guiEntry = new LibraryGUIEntry(this, rowLayout, entriesToDisplay[i], i);
                    entries[i] = guiEntry;
                    entryLookup[guiEntry.path] = guiEntry;

                    rowLayout.AddFlexibleSpace();

                    elemsInRow++;
                }

                int extraElements = elementsPerRow - elemsInRow;
                for (int i = 0; i < extraElements; i++)
                {
                    rowLayout.AddSpace(labelWidth);
                    rowLayout.AddFlexibleSpace();
                }

                main.AddFlexibleSpace();
            }

            for (int i = 0; i < entries.Length; i++)
            {
                LibraryGUIEntry guiEntry = entries[i];
                guiEntry.Initialize();
            }
        }

        public void MarkAsHovered(string path, bool hovered)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsHovered(hovered);
            }
        }

        public void MarkAsPinged(string path, bool pinged)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsPinged(pinged);
            }
        }

        public void MarkAsCut(string path, bool cut)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsCut(cut);
            }
        }

        public void MarkAsSelected(string path, bool selected)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsSelected(selected);
            }
        }

        public LibraryGUIEntry FindElementAt(Vector2I scrollPos)
        {
            foreach (var element in entries)
            {
                if (element.bounds.Contains(scrollPos))
                    return element;
            }

            return null;
        }

        public LibraryGUIEntry[] FindElementsOverlapping(Rect2I scrollBounds)
        {
            List<LibraryGUIEntry> elements = new List<LibraryGUIEntry>();
            foreach (var element in entries)
            {
                if (element.Bounds.Overlaps(scrollBounds))
                    elements.Add(element);
            }

            return elements.ToArray();
        }

        public bool TryGetEntry(string path, out LibraryGUIEntry entry)
        {
            return entryLookup.TryGetValue(path, out entry);
        }
    }
}
