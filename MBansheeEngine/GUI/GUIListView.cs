using System.Collections.Generic;

namespace BansheeEngine
{
    // TODO - Doc
    public class GUIListView<TEntry, TData> 
        where TEntry : GUIListViewEntry<TData>, new()
        where TData : GUIListViewData
    {
        // TODO - Only fixed size is supported. It should be nice if this object could just be placed in layout like any
        // other GUI element. Would likely need some kind of a way to get notified when parent layout changes.
        // (Possibly add a callback to GUIPanel when updateLayout is called?)

        private List<TEntry> visibleEntries = new List<TEntry>();
        private List<TData> entries = new List<TData>();
        private GUIScrollArea scrollArea;
        private GUILabel topPadding;
        private GUILabel bottomPadding;
        private int width;
        private int height;
        private int entryHeight;
        private float scrollPct = 0.0f;
        private bool contentsDirty = true;

        public int NumEntries
        {
            get { return entries.Count; }
        }

        public int EntryHeight
        {
            get { return entryHeight; }
            set { entryHeight = value; }
        }

        public GUIListView(int width, int height, int entryHeight, GUILayout layout)
        {
            scrollArea = new GUIScrollArea(GUIOption.FixedWidth(width), GUIOption.FixedHeight(height));
            layout.AddElement(scrollArea);

            topPadding = new GUILabel(new LocString());
            bottomPadding = new GUILabel(new LocString());

            scrollArea.Layout.AddElement(topPadding);
            scrollArea.Layout.AddElement(bottomPadding);

            this.width = width;
            this.height = height;
            this.entryHeight = entryHeight;
        }

        public void AddEntry(TData data)
        {
            entries.Add(data);
            contentsDirty = true;
        }

        public void RemoveEntry(int index)
        {
            if (index >= 0 && index < entries.Count)
            {
                entries.RemoveAt(index);
                contentsDirty = true;
            }
        }

        public void Clear()
        {
            entries.Clear();
            contentsDirty = true;
        }

        public int FindEntry(TData data)
        {
            return entries.FindIndex(x => x == data);
        }

        public void InsertEntry(int index, TData data)
        {
            if (index >= 0 && index <= entries.Count)
                entries.Insert(index, data);
            else
                entries.Add(data);

            contentsDirty = true;
        }

        public void SetSize(int width, int height)
        {
            if (width != this.width || height != this.height)
            {
                this.width = width;
                this.height = height;

                Rect2I bounds = scrollArea.Bounds;
                bounds.width = width;
                bounds.height = height;
                scrollArea.Bounds = bounds;
            }
        }

        public void Update()
        {
            int numVisibleEntries = MathEx.CeilToInt(height / (float)entryHeight) + 1;
            numVisibleEntries = MathEx.Min(numVisibleEntries, entries.Count);

            while (visibleEntries.Count < numVisibleEntries)
            {
                TEntry newEntry = new TEntry();
                newEntry.Initialize(scrollArea);
                newEntry.panel.SetHeight(entryHeight);

                visibleEntries.Add(newEntry);
                contentsDirty = true;
            }

            while (numVisibleEntries < visibleEntries.Count)
            {
                int lastIdx = visibleEntries.Count - 1;

                visibleEntries[lastIdx].Destroy();
                visibleEntries.RemoveAt(lastIdx);

                contentsDirty = true;
            }

            if (scrollPct != scrollArea.VerticalScroll)
            {
                scrollPct = scrollArea.VerticalScroll;
                contentsDirty = true;
            }

            if (contentsDirty)
            {
                int totalElementHeight = entries.Count*entryHeight;
                int scrollableHeight = MathEx.Max(0, totalElementHeight - height - 1);

                int startPos = MathEx.FloorToInt(scrollPct*scrollableHeight);
                int startIndex = startPos/entryHeight;

                topPadding.SetHeight(startIndex*entryHeight);

                for (int i = 0; i < visibleEntries.Count; i++)
                {
                    visibleEntries[i].UpdateContents(entries[startIndex + i]);
                    visibleEntries[i].panel.SetPosition(0, i * entryHeight);
                }

                int bottomPosition = (startIndex + visibleEntries.Count)*entryHeight;
                bottomPadding.SetHeight(totalElementHeight - bottomPosition);

                contentsDirty = false;
            }
        }
    }

    public class GUIListViewData
    {
        
    }

    public abstract class GUIListViewEntry<TData>
        where TData : GUIListViewData
    {
        internal GUIPanel panel;
        internal GUILayoutY layout;

        protected GUILayout Layout { get { return layout; } }

        internal void Initialize(GUIScrollArea parent)
        {
            int numElements = parent.Layout.ChildCount;

            // Last panel is always the padding panel, so keep it there
            panel = parent.Layout.InsertPanel(numElements - 1);
            layout = panel.AddLayoutY();

            BuildGUI();
        }

        internal void Destroy()
        {
            panel.Destroy();
        }

        public abstract void BuildGUI();
        public abstract void UpdateContents(TData data);
    }
}
