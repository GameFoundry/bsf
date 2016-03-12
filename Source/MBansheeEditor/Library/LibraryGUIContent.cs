//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.IO;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Manages GUI for the content area of the library window. Content area displays resources as a grid or list of
    /// resource icons.
    /// </summary>
    internal class LibraryGUIContent
    {
        private const int MIN_HORZ_SPACING = 8;
        private const int GRID_ENTRY_SPACING = 15;
        private const int LIST_ENTRY_SPACING = 7;

        private GUIPanel mainPanel;
        private GUILayout main;
        private GUIPanel overlay;
        private GUIPanel underlay;
        private GUIPanel deepUnderlay;
        private GUIPanel renameOverlay;

        private LibraryWindow window;
        private GUIScrollArea parent;
        private int tileSize;
        private bool gridLayout;

        private int elementsPerRow;
        private int labelWidth;
        private int elementWidth;

        private List<LibraryGUIEntry> entries = new List<LibraryGUIEntry>();
        private Dictionary<string, LibraryGUIEntry> entryLookup = new Dictionary<string, LibraryGUIEntry>();

        /// <summary>
        /// Area of the content area relative to the parent window.
        /// </summary>
        public Rect2I Bounds
        {
            get { return main.Bounds; }
        }

        /// <summary>
        /// Number of elements per row. Only relevant for grid layouts.
        /// </summary>
        public int ElementsPerRow
        {
            get { return elementsPerRow; }
        }

        /// <summary>
        /// Width of a single element in a row, including any margins. Only relevant for grid layouts.
        /// </summary>
        public int ElementWidth
        {
            get { return elementWidth; }
        }

        /// <summary>
        /// Determines is the content display in a grid (true) or list (false) layout.
        /// </summary>
        public bool GridLayout
        {
            get { return gridLayout; }
        }

        /// <summary>
        /// Sizes of a single resource tile in grid layout. Size in list layout is fixed.
        /// </summary>
        public int TileSize
        {
            get { return tileSize; }
        }

        /// <summary>
        /// Returns objects representing each of the displayed resource icons.
        /// </summary>
        public List<LibraryGUIEntry> Entries
        {
            get { return entries; }
        }

        /// <summary>
        /// Returns parent window the content area is part of.
        /// </summary>
        public LibraryWindow Window
        {
            get { return window; }
        }

        /// <summary>
        /// Returns a GUI panel that can be used for displaying elements underneath the resource tiles.
        /// </summary>
        public GUIPanel Underlay
        {
            get { return underlay; }
        }

        /// <summary>
        /// Returns a GUI panel that can be used for displaying elements underneath the resource tiles. Displays under
        /// <see cref="Underlay"/>.
        /// </summary>
        public GUIPanel DeepUnderlay
        {
            get { return deepUnderlay; }
        }

        /// <summary>
        /// Returns a GUI panel that can be used for displaying elements above the resource tiles.
        /// </summary>
        public GUIPanel Overlay
        {
            get { return overlay; }
        }

        /// <summary>
        /// Returns a GUI panel that can be used for displaying rename input box. Displayed on top of the resource tiles
        /// and the standard overlay.
        /// </summary>
        public GUIPanel RenameOverlay
        {
            get { return renameOverlay; }
        }

        /// <summary>
        /// Constructs a new GUI library content object.
        /// </summary>
        /// <param name="window">Parent window the content area is part of.</param>
        /// <param name="parent">Scroll area the content area is part of.</param>
        public LibraryGUIContent(LibraryWindow window, GUIScrollArea parent)
        {
            this.window = window;
            this.parent = parent;
        }

        /// <summary>
        /// Refreshes the contents of the content area. Must be called at least once after construction.
        /// </summary>
        /// <param name="viewType">Determines how to display the resource tiles.</param>
        /// <param name="entriesToDisplay">Project library entries to display.</param>
        public void Refresh(ProjectViewType viewType, LibraryEntry[] entriesToDisplay)
        {
            if (mainPanel != null)
                mainPanel.Destroy();

            entries.Clear();
            entryLookup.Clear();

            mainPanel = parent.Layout.AddPanel();

            GUIPanel contentPanel = mainPanel.AddPanel(1);
            overlay = mainPanel.AddPanel(0);
            underlay = mainPanel.AddPanel(2);
            deepUnderlay = mainPanel.AddPanel(3);
            renameOverlay = mainPanel.AddPanel(-1);

            main = contentPanel.AddLayoutY();

            List<ResourceToDisplay> resourcesToDisplay = new List<ResourceToDisplay>();
            foreach (var entry in entriesToDisplay)
            {
                if (entry.Type == LibraryEntryType.Directory)
                    resourcesToDisplay.Add(new ResourceToDisplay(entry.Path, LibraryGUIEntryType.Single));
                else
                {
                    FileEntry fileEntry = (FileEntry)entry;
                    ResourceMeta[] metas = fileEntry.ResourceMetas;

                    if (metas.Length > 0)
                    {
                        if (metas.Length == 1)
                            resourcesToDisplay.Add(new ResourceToDisplay(entry.Path, LibraryGUIEntryType.Single));
                        else
                        {
                            resourcesToDisplay.Add(new ResourceToDisplay(entry.Path, LibraryGUIEntryType.MultiFirst));

                            for (int i = 1; i < metas.Length - 1; i++)
                            {
                                string path = Path.Combine(entry.Path, metas[i].SubresourceName);
                                resourcesToDisplay.Add(new ResourceToDisplay(path, LibraryGUIEntryType.MultiElement));
                            }

                            string lastPath = Path.Combine(entry.Path, metas[metas.Length - 1].SubresourceName);
                            resourcesToDisplay.Add(new ResourceToDisplay(lastPath, LibraryGUIEntryType.MultiLast));
                        }
                    }
                }
            }

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

                int numRows = MathEx.CeilToInt(resourcesToDisplay.Count / (float)elementsPerRow);
                int neededHeight = numRows * (elemSize);

                bool requiresScrollbar = neededHeight > scrollBounds.height;
                if (requiresScrollbar)
                {
                    availableWidth -= parent.ScrollBarWidth;
                    elementsPerRow = (availableWidth - GRID_ENTRY_SPACING * 2) / elemSize;
                }

                if (elementsPerRow > 0)
                {
                    labelWidth = (availableWidth - (elementsPerRow + 1)*MIN_HORZ_SPACING)/elementsPerRow;
                    elementWidth = availableWidth/elementsPerRow;
                }
                else
                {
                    labelWidth = 0;
                    elementWidth = 0;
                }
            }

            if (viewType == ProjectViewType.List16)
            {
                for (int i = 0; i < resourcesToDisplay.Count; i++)
                {
                    ResourceToDisplay entry = resourcesToDisplay[i];

                    LibraryGUIEntry guiEntry = new LibraryGUIEntry(this, main, entry.path, i, labelWidth, entry.type);
                    entries.Add(guiEntry);
                    entryLookup[guiEntry.path] = guiEntry;

                    if (i != resourcesToDisplay.Count - 1)
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

                for (int i = 0; i < resourcesToDisplay.Count; i++)
                {
                    if (elemsInRow == elementsPerRow && elemsInRow > 0)
                    {
                        rowLayout = main.AddLayoutX();
                        main.AddSpace(GRID_ENTRY_SPACING);

                        rowLayout.AddFlexibleSpace();
                        elemsInRow = 0;
                    }

                    ResourceToDisplay entry = resourcesToDisplay[i];

                    LibraryGUIEntry guiEntry = new LibraryGUIEntry(this, rowLayout, entry.path, i, labelWidth, entry.type);
                    entries.Add(guiEntry);
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

            for (int i = 0; i < entries.Count; i++)
            {
                LibraryGUIEntry guiEntry = entries[i];
                guiEntry.Initialize();
            }
        }

        /// <summary>
        /// Called every frame.
        /// </summary>
        public void Update()
        {
            for (int i = 0; i < entries.Count; i++)
                entries[i].Update();
        }

        /// <summary>
        /// Changes the visual representation of an element at the specified path as being hovered over.
        /// </summary>
        /// <param name="path">Project library path to the element to mark.</param>
        /// <param name="hovered">True if mark as hovered, false to reset to normal.</param>
        public void MarkAsHovered(string path, bool hovered)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsHovered(hovered);
            }
        }

        /// <summary>
        /// Changes the visual representation of an element at the specified path as being pinged.
        /// </summary>
        /// <param name="path">Project library path to the element to mark.</param>
        /// <param name="pinged">True if mark as pinged, false to reset to normal.</param>
        public void MarkAsPinged(string path, bool pinged)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsPinged(pinged);
            }
        }

        /// <summary>
        /// Changes the visual representation of an element at the specified path as being cut.
        /// </summary>
        /// <param name="path">Project library path to the element to mark.</param>
        /// <param name="cut">True if mark as cut, false to reset to normal.</param>
        public void MarkAsCut(string path, bool cut)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsCut(cut);
            }
        }

        /// <summary>
        /// Changes the visual representation of an element at the specified path as being selected.
        /// </summary>
        /// <param name="path">Project library path to the element to mark.</param>
        /// <param name="selected">True if mark as selected, false to reset to normal.</param>
        public void MarkAsSelected(string path, bool selected)
        {
            if (!string.IsNullOrEmpty(path))
            {
                LibraryGUIEntry previousUnderCursorElem;
                if (entryLookup.TryGetValue(path, out previousUnderCursorElem))
                    previousUnderCursorElem.MarkAsSelected(selected);
            }
        }

        /// <summary>
        /// Attempts to find a resource tile element at the specified coordinates.
        /// </summary>
        /// <param name="scrollPos">Coordinates relative to the scroll area the content area is part of.</param>
        /// <returns>True if found an entry, false otherwise.</returns>
        public LibraryGUIEntry FindElementAt(Vector2I scrollPos)
        {
            foreach (var element in entries)
            {
                if (element.bounds.Contains(scrollPos))
                    return element;
            }

            return null;
        }

        /// <summary>
        /// Attempts to find all resource tile elements overlapping the specified bounds.
        /// </summary>
        /// <param name="scrollBounds">Bounds to check for overlap, specified relative to the scroll area the content area 
        ///                            is part of.</param>
        /// <returns>A list of found entries.</returns>
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

        /// <summary>
        /// Attempts to find a resource tile element with the specified path.
        /// </summary>
        /// <param name="path">Project library path to the element.</param>
        /// <param name="entry">Found element, or null if none found.</param>
        /// <returns>True if an element was found, false otherwise.</returns>
        public bool TryGetEntry(string path, out LibraryGUIEntry entry)
        {
            return entryLookup.TryGetValue(path, out entry);
        }

        /// <summary>
        /// Helper structure containing information about a single entry to display in the library.
        /// </summary>
        private struct ResourceToDisplay
        {
            public ResourceToDisplay(string path, LibraryGUIEntryType type)
            {
                this.path = path;
                this.type = type;
            }

            public string path;
            public LibraryGUIEntryType type;
        }
    }
}
