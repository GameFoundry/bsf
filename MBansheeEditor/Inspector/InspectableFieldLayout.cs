using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableFieldLayout
    {
        private GUILayoutY parentLayout;
        private List<GUIElement> elements = new List<GUIElement>();

        public InspectableFieldLayout(GUILayoutY parentLayout)
        {
            this.parentLayout = parentLayout;
        }

        public int GetNumElements() { return elements.Count; }

        public void AddElement(int index, GUIElement element)
        {
            parentLayout.InsertElement(index + elements.Count, element);
            elements.Add(element);
        }

        public GUILayoutX AddLayoutX(int index)
        {
            GUILayoutX layout = parentLayout.InsertLayoutX(index + elements.Count);
            elements.Add(layout);

            return layout;
        }

        public GUILayoutY AddLayoutY(int index)
        {
            GUILayoutY layout = parentLayout.InsertLayoutY(index + elements.Count);
            elements.Add(layout);

            return layout;
        }

        public void DestroyElements()
        {
            foreach (var element in elements)
            {
                element.Destroy();
            }

            elements.Clear();
        }
    }
}
