using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUIBase : ScriptObject
    {
        private GUIArea mainArea;
        private GUILayout _mainLayout;

        internal List<GUIArea> childAreas = new List<GUIArea>();

        public GUILayout layout
        {
            get { return _mainLayout; }
        }

        public GUISkin skin; // TODO

        internal GUIBase()
        { }

        ~GUIBase()
        {
            for (int i = 0; i < childAreas.Count; i++)
            {
                childAreas[i].Destroy();
            }

            childAreas.Clear();
        }

        internal void Initialize()
        {
            mainArea = AddResizableAreaXY(0, 0, 0, 0);
            _mainLayout = mainArea.layout;
        }

        public GUIArea AddArea(int x, int y, int width = 0, int height = 0, short depth = 0)
        {
            GUIArea area = GUIArea.Create(this, x, y, width, height, depth);
            childAreas.Add(area);

            return area;
        }

        public GUIArea AddResizableAreaX(int offsetLeft, int offsetRight, int offsetTop, int height, short depth = 0)
        {
            GUIArea area = GUIArea.CreateResizableX(this, offsetLeft, offsetRight, offsetTop, height, depth);
            childAreas.Add(area);

            return area;
        }

        public GUIArea AddResizableAreaY(int offsetTop, int offsetBottom, int offsetLeft, int width, short depth = 0)
        {
            GUIArea area = GUIArea.CreateResizableY(this, offsetTop, offsetBottom, offsetLeft, width, depth);
            childAreas.Add(area);

            return area;
        }

        public GUIArea AddResizableAreaXY(int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, short depth = 0)
        {
            GUIArea area = GUIArea.CreateResizableXY(this, offsetLeft, offsetRight, offsetTop, offsetBottom, depth);
            childAreas.Add(area);

            return area;
        }
    }
}
