using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectorArea
    {
        private GUIArea guiArea;
        private Inspector owner;

        private int parentX, parentY, parentWidth, parentHeight;
        private int x, y, width, height;
        private short depth;

        public GUILayout layout
        {
            get { return guiArea.layout; }
        }

        internal InspectorArea(Inspector owner, EditorGUI parentGUI)
        {
            this.owner = owner;
            guiArea = parentGUI.AddArea(0, 0, 0, 0);
        }

        public void Destroy()
        {
            owner.Remove(this);

            guiArea.Destroy();
        }

        public void SetArea(int x, int y, int width, int height, short depth)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
            this.depth = depth;

            UpdateGUIArea();
        }

        internal void SetParentArea(int x, int y, int width, int height)
        {
            parentX = x;
            parentY = y;
            parentWidth = width;
            parentHeight = height;

            UpdateGUIArea();
        }

        private void UpdateGUIArea()
        {
            int guiX = parentX + x;
            int guiY = parentY + y;

            int guiRight = guiX + width;
            guiRight = MathEx.Min(guiRight, parentX + parentWidth);

            int guiWidth = MathEx.Max(0, guiRight - guiX);

            int guiBottom = guiY + height;
            guiBottom = MathEx.Min(guiBottom, parentY + parentHeight);

            int guiHeight = MathEx.Max(0, guiBottom - guiY);

            guiArea.SetArea(guiX, guiY, guiWidth, guiHeight, depth);
        }
    }
}
